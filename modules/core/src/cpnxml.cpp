#include "cpnxml.hpp"

#include "logger.hpp"

_START_PSM_NM_

CPNXml::CPNXml(::std::string name) : name_(name)
{
    InitTopLevel();
};

void CPNXml::Dump() const
{
    auto file = name_ + FILE_EXT;
    doc_.save_file(file.c_str());
}

int CPNXml::MakeId()
{
    return id_++;
}

void CPNXml::InitTopLevel()
{
    // <?xml ...>
    decl_ = doc_.prepend_child(pugi::node_declaration);
    decl_.append_attribute(Key(VERSION)) = Value(VERSION);
    decl_.append_attribute(Key(ENCODING)) = Value(ENCODING);

    // <!DOCTYPE ...>
    doctype_ = doc_.append_child(pugi::node_doctype);
    doctype_.set_value(DOCTYPE_VALUE);

    // <workspaceElements>
    InitWorkspaceElements();
}

void CPNXml::InitWorkspaceElements()
{
    workspaceElements_ = doc_.append_child(WORKSPACE_ELEMENTS);
    // <generator ...>
    generator_ = workspaceElements_.append_child(GENERATOR);
    generator_.append_attribute(Key(GENERATOR_TOOL)) = Value(GENERATOR_TOOL);
    generator_.append_attribute(Key(GENERATOR_FORMAT)) = Value(GENERATOR_FORMAT);

    // <cpnet>
    InitCpnet();
}

void CPNXml::InitCpnet()
{
    cpnet_ = workspaceElements_.append_child(CPNET);

    // <globbox>
    globbox_ = cpnet_.append_child(GLOBBOX);
    sdBlock_ = blocks_[AddBlock(SD_BLOCK_NAME)];
    spBlock_ = blocks_[AddBlock(SP_BLOCK_NAME)];

    // <options>
    options_ = cpnet_.append_child(OPTIONS);

    // <binders> (self-closing expected)
    binders_ = cpnet_.append_child(BINDERS);

    // <monitorblock ...>
    monitorblock_ = cpnet_.append_child(MONITOR_BLOCK);
    monitorblock_.append_attribute(Key(MONITOR_BLOCK_NAME)) = Value(MONITOR_BLOCK_NAME);

    // <IndexNode ...> which is an optional node
    indexNode_ = cpnet_.append_child(INDEX_NODE);
    indexNode_.append_attribute(Key(INDEX_NODE_EXPANDED)) = Value(INDEX_NODE_EXPANDED);

    // <instances>
    instances_ = cpnet_.append_child(INSTANCES);
}

int CPNXml::AddPage(::std::string name)
{
    int id = MakeId();
    pugi::xml_node page = cpnet_.append_child(PAGE);
    page.append_attribute(ID) = ::std::to_string(id).c_str();

    // <pageattr ...>
    pugi::xml_node pageattr = page.append_child(PAGEATTR);
    pageattr.append_attribute(NAME) = name.c_str();

    pages_.insert(::std::make_pair(id, page));
    return id;
}

int CPNXml::AddBlock(::std::string name)
{
    int id = MakeId();
    pugi::xml_node block = globbox_.append_child(BLOCK);
    block.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = block.append_child(ID);
    idnode.text().set(name.c_str());

    blocks_.insert(::std::make_pair(id, block));
    return id;
}

void CPNXml::DeclareUnitColorSet(
    ::std::string name,
    ::std::optional<::std::string> new_unit)
{
    int id = MakeId();
    const char *UNIT_DECL_LAYOUT_FMT;
    if (new_unit)
        UNIT_DECL_LAYOUT_FMT = "colset %s = unit with %s;";
    else
        UNIT_DECL_LAYOUT_FMT = "colset %s = unit;";
    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <unit>
    pugi::xml_node unit = color.append_child(UNIT);
    unit.text().set("\n");
    if (new_unit)
    {
        pugi::xml_node with = unit.append_child(WITH);
        pugi::xml_node idnode2 = with.append_child(ID);
        idnode2.text().set(new_unit->c_str());
    }

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];
    if (new_unit)
        sprintf(buf, UNIT_DECL_LAYOUT_FMT, name.c_str(), new_unit->c_str());
    else
        sprintf(buf, UNIT_DECL_LAYOUT_FMT, name.c_str());
    layout.text().set(buf);
}

void CPNXml::DeclareBooleanColorSet(
    ::std::string name,
    ::std::optional<::std::tuple<::std::string, ::std::string>> new_false_true)
{
    int id = MakeId();
    const char *BOOLEAN_DECL_LAYOUT_FMT;
    if (new_false_true)
        BOOLEAN_DECL_LAYOUT_FMT = "colset %s = bool with (%s, %s);";
    else
        BOOLEAN_DECL_LAYOUT_FMT = "colset %s = bool;";
    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <bool>
    pugi::xml_node boolnode = color.append_child(BOOL);
    boolnode.text().set("\n");
    if (new_false_true)
    {
        pugi::xml_node with = boolnode.append_child(WITH);
        pugi::xml_node idnode2 = with.append_child(ID);
        idnode2.text().set(::std::get<0>(*new_false_true).c_str());
        pugi::xml_node idnode3 = with.append_child(ID);
        idnode3.text().set(::std::get<1>(*new_false_true).c_str());
    }

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];
    if (new_false_true)
        sprintf(buf, BOOLEAN_DECL_LAYOUT_FMT, name.c_str(), ::std::get<0>(*new_false_true).c_str(), ::std::get<1>(*new_false_true).c_str());
    else
        sprintf(buf, BOOLEAN_DECL_LAYOUT_FMT, name.c_str());
    layout.text().set(buf);
}

void CPNXml::DeclareIntegerColorSet(
    ::std::string name,
    ::std::optional<::std::string> int_expr)
{
    int id = MakeId();
    const char *INTEGER_DECL_LAYOUT_FMT;
    if (int_expr)
        // FIXME: with clause in Integer color set decl is a little complicated
        // Will follow up implementation later on...
        INTEGER_DECL_LAYOUT_FMT = "colset %s = int;";
    else
        INTEGER_DECL_LAYOUT_FMT = "colset %s = int;";
    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <int>
    pugi::xml_node intnode = color.append_child(INT);
    intnode.text().set("\n");
    if (int_expr)
    {
        // TODO: Handle with clause...
    }

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];
    if (int_expr)
        // TODO: Handle with clause...
        sprintf(buf, INTEGER_DECL_LAYOUT_FMT, name.c_str());
    else
        sprintf(buf, INTEGER_DECL_LAYOUT_FMT, name.c_str());
    layout.text().set(buf);
};

void CPNXml::DeclareLargeIntegerColorSet(
    ::std::string name,
    ::std::optional<::std::string> int_expr)
{
    int id = MakeId();
    const char *LARGEINTEGER_DECL_LAYOUT_FMT;
    if (int_expr)
        // FIXME: with clause in Integer color set decl is a little complicated
        // Will follow up implementation later on...
        LARGEINTEGER_DECL_LAYOUT_FMT = "colset %s = intinf;";
    else
        LARGEINTEGER_DECL_LAYOUT_FMT = "colset %s = intinf;";
    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <intinf>
    pugi::xml_node intinf = color.append_child(INTINF);
    intinf.text().set("\n");
    if (int_expr)
    {
        // TODO: Handle with clause...
    }

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];
    if (int_expr)
        // TODO: Handle with clause...
        sprintf(buf, LARGEINTEGER_DECL_LAYOUT_FMT, name.c_str());
    else
        sprintf(buf, LARGEINTEGER_DECL_LAYOUT_FMT, name.c_str());
    layout.text().set(buf);
};

void CPNXml::DeclareRealColorSet(
    ::std::string name,
    ::std::optional<::std::string> real_expr)
{
    int id = MakeId();
    const char *REAL_DECL_LAYOUT_FMT;
    if (real_expr)
        // FIXME: with clause in Real color set decl is a little complicated
        // Will follow up implementation later on...
        REAL_DECL_LAYOUT_FMT = "colset %s = real;";
    else
        REAL_DECL_LAYOUT_FMT = "colset %s = real;";
    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <real>
    pugi::xml_node real = color.append_child(REAL);
    real.text().set("\n");
    if (real_expr)
    {
        // TODO: Handle with clause...
    }

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];
    if (real_expr)
        // TODO: Handle with clause...
        sprintf(buf, REAL_DECL_LAYOUT_FMT, name.c_str());
    else
        sprintf(buf, REAL_DECL_LAYOUT_FMT, name.c_str());
    layout.text().set(buf);
};

void CPNXml::DeclareTimeColorSet(
    ::std::string name)
{
    int id = MakeId();
    const char *TIME_DECL_LAYOUT_FMT;
    TIME_DECL_LAYOUT_FMT = "colset %s = time;";
    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <time>
    pugi::xml_node timenode = color.append_child(TIME);
    timenode.text().set("\n");

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];
    sprintf(buf, TIME_DECL_LAYOUT_FMT, name.c_str());
    layout.text().set(buf);
};

void CPNXml::DeclareStringColorSet(
    ::std::string name,
    ::std::optional<::std::string> string_expr,
    ::std::optional<::std::string> int_expr)
{
    int id = MakeId();
    const char *STRING_DECL_LAYOUT_FMT;
    if (string_expr)
    {
        // FIXME: `with` and `and` clause in String color set decl is a little complicated
        // Will follow up implementation later on...
        if (int_expr)
        {
            STRING_DECL_LAYOUT_FMT = "colset %s = string;";
        }
        else
        {
            STRING_DECL_LAYOUT_FMT = "colset %s = string;";
        }
    }
    else
        STRING_DECL_LAYOUT_FMT = "colset %s = string;";

    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <string>
    pugi::xml_node stringnode = color.append_child(STRING);
    stringnode.text().set("\n");
    if (string_expr)
    {
        // TODO: Handle with clause...
        if (int_expr)
        {
            // TODO: Handle and clause...
        }
    }

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];
    if (string_expr)
    {
        // TODO: Handle with clause...
        if (int_expr)
        {
            // TODO: Handle and clause...
            sprintf(buf, STRING_DECL_LAYOUT_FMT, name.c_str());
        }
        else
        {
            sprintf(buf, STRING_DECL_LAYOUT_FMT, name.c_str());
        }
    }
    else
        sprintf(buf, STRING_DECL_LAYOUT_FMT, name.c_str());
    layout.text().set(buf);
};

void CPNXml::DeclareEnumeratedColorSet(
    ::std::string name,
    ::std::vector<::std::string> ids)
{
    int id = MakeId();
    const char *ENUMERATED_DECL_LAYOUT_FMT;
    ENUMERATED_DECL_LAYOUT_FMT = "colset %s = with %s;";

    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <enum>
    pugi::xml_node enumnode = color.append_child(ENUMERATE);
    enumnode.text().set("\n");

    // Handle ids
    ::std::string sid = "";

    // Integrity check
    if (ids.empty())
    {
        // Should be unreachable...
        LOGE("DeclareEnumeratedColorSet does not accept empty ids...");
        PSM_BAIL();
    }

    // Assemble ids
    for (auto &x : ids)
    {
        pugi::xml_node idnode2 = enumnode.append_child(ID);
        idnode2.text().set(x.c_str());
        sid += x;
        sid += '|';
    }

    // Trim last '|'
    sid = sid.substr(0, sid.size() - 1);

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];

    sprintf(buf, ENUMERATED_DECL_LAYOUT_FMT, name.c_str(), sid.c_str());
    layout.text().set(buf);
};

void CPNXml::DeclareIndexColorSet(
    ::std::string name,
    ::std::string int_expr){
    // FIXME: Implementation for DeclareIndexColorSet is a little complicated
    // Moreover this is not a regularly used decl.
    // Will follow up implementation later...
};

// Compound color sets (using previously CPNXml::Declared color sets)
void CPNXml::DeclareProductColorSet(
    ::std::string name,
    ::std::vector<::std::string> components)
{
    int id = MakeId();
    const char *PRODUCT_DECL_LAYOUT_FMT;
    PRODUCT_DECL_LAYOUT_FMT = "colset %s = product %s;";

    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <product>
    pugi::xml_node product = color.append_child(PRODUCT);
    product.text().set("\n");

    // Handle ids
    ::std::string spros = "";

    // Integrity check
    if (components.empty())
    {
        // Should be unreachable...
        LOGE("DeclareProductColorSet does not accept empty components...");
        PSM_BAIL();
    }

    // Assemble components
    for (auto &x : components)
    {
        pugi::xml_node idnode2 = product.append_child(ID);
        idnode2.text().set(x.c_str());
        spros += x;
        spros += '*';
    }

    // Trim last '*'
    spros = spros.substr(0, spros.size() - 1);

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];

    sprintf(buf, PRODUCT_DECL_LAYOUT_FMT, name.c_str(), spros.c_str());
    layout.text().set(buf);
};

void CPNXml::DeclareAliasColorSets(
    ::std::string name,
    ::std::string name0)
{
    int id = MakeId();
    const char *ALIAS_DECL_LAYOUT_FMT = "colset %s = %s;";
    pugi::xml_node color = sdBlock_.append_child(COLOR);
    color.append_attribute(ID) = ::std::to_string(id).c_str();

    // <id>
    pugi::xml_node idnode = color.append_child(ID);
    idnode.text().set(name.c_str());

    // <alias>
    pugi::xml_node alias = color.append_child(ALIAS);
    pugi::xml_node idnode2 = alias.append_child(ID);
    idnode2.text().set(name0.c_str());

    // <layout>
    pugi::xml_node layout = color.append_child(LAYOUT);
    char buf[256];
    sprintf(buf, ALIAS_DECL_LAYOUT_FMT, name.c_str(), name0.c_str());
    layout.text().set(buf);
}

void CPNXml::DeclareVar(
    ::std::string name,
    ::std::string cs_name)
{
    int id = MakeId();
    pugi::xml_node variable = sdBlock_.append_child(VARIABLE);
    variable.append_attribute(ID) = ::std::to_string(id).c_str();

    // <type>
    pugi::xml_node type = variable.append_child(TYPE);
    pugi::xml_node idnode = type.append_child(ID);
    idnode.text().set(cs_name.c_str());

    // <id>
    pugi::xml_node idnode2 = variable.append_child(ID);
    idnode2.text().set(name.c_str());
}

void CPNXml::DeclareVar(
    ::std::vector<::std::string> names,
    ::std::string cs_name)
{
    int id = MakeId();
    pugi::xml_node variable = sdBlock_.append_child(VARIABLE);
    variable.append_attribute(ID) = ::std::to_string(id).c_str();

    // <type>
    pugi::xml_node type = variable.append_child(TYPE);
    pugi::xml_node idnode = type.append_child(ID);
    idnode.text().set(cs_name.c_str());

    // <id>
    for (auto &x : names)
    {
        pugi::xml_node idnode2 = variable.append_child(ID);
        idnode2.text().set(x.c_str());
    }
}

_END_PSM_NM_