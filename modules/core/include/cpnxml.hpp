#include "base.hpp"

#include <string>
#include <map>

#include "pugixml.hpp"

_START_PSM_NM_

class CPNXml {
public:
    CPNXml(::std::string name);

    void Dump() const;

    int AddPage(::std::string name);

private:
    template<typename T1, typename T2>
    const T1& Key(const ::std::pair<T1, T2>& p) {
        return p.first;
    }

    template<typename T1, typename T2>
    const T2& Value(const ::std::pair<T1, T2>& p) {
        return p.second;
    }

    int MakeId();

    void InitTopLevel();

    void InitWorkspaceElements();

    void InitCpnet();

private:
    ::std::string name_;
    int id_{0};
    pugi::xml_document doc_;
    pugi::xml_node decl_;
    pugi::xml_node doctype_;
    pugi::xml_node workspaceElements_;
    pugi::xml_node generator_;
    pugi::xml_node cpnet_;
    pugi::xml_node globbox_;
    pugi::xml_node options_;
    pugi::xml_node binders_;
    pugi::xml_node monitorblock_;
    pugi::xml_node indexNode_;
    pugi::xml_node instances_;
    ::std::map<int, pugi::xml_node> pages_;

private:
    static constexpr const char* FILE_EXT = ".cpn";
    static constexpr ::std::pair<const char*, const char*> VERSION = {"version", "1.0"};
    static constexpr ::std::pair<const char*, const char*> ENCODING = {"encoding", "iso-8859-1"};
    static constexpr const char* DOCTYPE_VALUE = "workspaceElements PUBLIC \"-//CPN//DTD CPNXML 1.0//EN\" \"http://cpntools.org/DTD/6/cpn.dtd\"";
    static constexpr const char* WORKSPACE_ELEMENTS = "workspaceElements";
    static constexpr const char* GENERATOR = "generator";
    static constexpr ::std::pair<const char*, const char*> GENERATOR_TOOL = {"tool", "CPN Tools"};
    static constexpr ::std::pair<const char*, const char*> GENERATOR_VERSION = {"version", "4.0.1"};
    static constexpr ::std::pair<const char*, const char*> GENERATOR_FORMAT = {"format", "6"};
    static constexpr const char* CPNET = "cpnet";
    static constexpr const char* GLOBBOX = "globbox";
    static constexpr const char* OPTIONS = "options";
    static constexpr const char* BINDERS = "binders";
    static constexpr const char* INSTANCES = "instances";
    static constexpr const char* MONITOR_BLOCK = "monitorblock";
    static constexpr ::std::pair<const char*, const char*> MONITOR_BLOCK_NAME = {"name", "Monitors"};
    static constexpr const char* INDEX_NODE = "IndexNode";
    static constexpr ::std::pair<const char*, const char*> INDEX_NODE_EXPANDED = {"expanded", "false"};

    // General component attributes
    static constexpr const char* ID = "id";
    static constexpr const char* NAME = "name";

    // Page attribs
    static constexpr const char* PAGE = "page";
    static constexpr const char* PAGEATTR = "pageattr";

    // Predefined basic types
    static constexpr const char* UNIT = "unit";
    static constexpr const char* BOOL = "bool";
    static constexpr const char* INT = "int";
    static constexpr const char* INTINF = "intinf";
    static constexpr const char* TIME = "time";
    static constexpr const char* REAL = "real";
    static constexpr const char* STRING = "string";

    // Compound types
    static constexpr const char* PRODUCT = "product";
};

_END_PSM_NM_