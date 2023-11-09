#include "cpnxml.hpp"

_START_PSM_NM_

CPNXml::CPNXml(::std::string name) : name_(name)
{
    InitTopLevel();
};

void CPNXml::Dump() const {
    auto file = name_ + FILE_EXT;
    doc_.save_file(file.c_str());
}

int CPNXml::MakeId() {
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

int CPNXml::AddPage(::std::string name) {
    int id = MakeId();
    pugi::xml_node page = cpnet_.append_child(PAGE);
    page.append_attribute(ID) = ::std::to_string(id).c_str();

    // <pageattr ...>
    pugi::xml_node pageattr = page.append_child(PAGEATTR);
    pageattr.append_attribute(NAME) = name.c_str();

    return id;
}

_END_PSM_NM_