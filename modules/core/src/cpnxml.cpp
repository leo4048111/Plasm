#include "cpnxml.hpp"

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
    int blockId = AddBlock(SD_BLOCK_NAME);
    sdBlock_ = blocks_[blockId];

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
}

void CPNXml::DeclareBooleanColorSet(
    ::std::string name,
    ::std::optional<::std::tuple<::std::string, ::std::string>> new_false_true)
{
}

void CPNXml::DeclareIntegerColorSet(
    ::std::string name,
    ::std::optional<::std::string> int_expr){

};

void CPNXml::DeclareLargeIntegerColorSet(
    ::std::string name,
    ::std::optional<::std::string> int_expr){};

void CPNXml::DeclareRealColorSet(
    ::std::string name,
    ::std::optional<::std::string> real_expr){};

void CPNXml::DeclareTimeColorSet(
    ::std::string name){};

void CPNXml::DeclareStringColorSet(
    ::std::string name,
    ::std::optional<::std::string> string_expr,
    ::std::optional<::std::string> int_expr){};

void CPNXml::DeclareEnumeratedColorSet(
    ::std::string name,
    ::std::vector<::std::string> ids){};

void CPNXml::DeclareIndexColorSet(
    ::std::string name,
    ::std::string int_expr){};

// Compound color sets (using previously CPNXml::Declared color sets)
void CPNXml::DeclareProductColorSet(
    ::std::string name,
    ::std::vector<::std::string> components){};

void CPNXml::DeclareAliasColorSets(
    ::std::string name,
    ::std::string name0)
{
    int id = MakeId();
    static const char* ALIAS_DECL_LAYOUT_FMT = "colset %s = %s;";
    pugi::xml_node color = sdBlock_.append_child(COLOR);

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

_END_PSM_NM_