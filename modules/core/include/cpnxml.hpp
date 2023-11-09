#include "base.hpp"

#include <string>
#include <map>
#include <optional>
#include <vector>

#include "pugixml.hpp"

_START_PSM_NM_

class CPNXml
{
public:
    CPNXml(::std::string name);

    void Dump() const;

    // Add a page to main document
    int AddPage(::std::string name);

    // See definition https://cpntools.org/2018/01/12/color-sets/
    // Simple color sets
    void DeclareUnitColorSet(
        ::std::string name,
        ::std::optional<::std::string> new_unit = ::std::nullopt);

    void DeclareBooleanColorSet(
        ::std::string name,
        ::std::optional<::std::tuple<::std::string, ::std::string>> new_false_true = ::std::nullopt);

    void DeclareIntegerColorSet(
        ::std::string name,
        ::std::optional<::std::string> int_expr = ::std::nullopt);

    void DeclareLargeIntegerColorSet(
        ::std::string name,
        ::std::optional<::std::string> int_expr = ::std::nullopt);

    void DeclareRealColorSet(
        ::std::string name,
        ::std::optional<::std::string> real_expr = ::std::nullopt);

    void DeclareTimeColorSet(
        ::std::string name);

    void DeclareStringColorSet(
        ::std::string name,
        ::std::optional<::std::string> string_expr = ::std::nullopt,
        ::std::optional<::std::string> int_expr = ::std::nullopt);

    void DeclareEnumeratedColorSet(
        ::std::string name,
        ::std::vector<::std::string> ids);

    void DeclareIndexColorSet(
        ::std::string name,
        ::std::string int_expr);

    // Compound color sets (using previously declared color sets)
    void DeclareProductColorSet(
        ::std::string name,
        ::std::vector<::std::string> components);

    void DeclareAliasColorSets(
        ::std::string name,
        ::std::string name0);

private:
    template <typename T1, typename T2>
    const T1 &Key(const ::std::pair<T1, T2> &p)
    {
        return p.first;
    }

    template <typename T1, typename T2>
    const T2 &Value(const ::std::pair<T1, T2> &p)
    {
        return p.second;
    }

    int MakeId();

    void InitTopLevel();

    void InitWorkspaceElements();

    void InitCpnet();

    // Note: Currently not exposing AddBlock as an interface, only Standard priorities is required 
    int AddBlock(::std::string name);

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
    pugi::xml_node sdBlock_;
    pugi::xml_node options_;
    pugi::xml_node binders_;
    pugi::xml_node monitorblock_;
    pugi::xml_node indexNode_;
    pugi::xml_node instances_;
    ::std::map<int, pugi::xml_node> pages_;
    ::std::map<int, pugi::xml_node> blocks_;

private:
    static constexpr const char *FILE_EXT = ".cpn";
    static constexpr ::std::pair<const char *, const char *> VERSION = {"version", "1.0"};
    static constexpr ::std::pair<const char *, const char *> ENCODING = {"encoding", "iso-8859-1"};
    static constexpr const char *DOCTYPE_VALUE = "workspaceElements PUBLIC \"-//CPN//DTD CPNXML 1.0//EN\" \"http://cpntools.org/DTD/6/cpn.dtd\"";
    static constexpr const char *WORKSPACE_ELEMENTS = "workspaceElements";
    static constexpr const char *GENERATOR = "generator";
    static constexpr ::std::pair<const char *, const char *> GENERATOR_TOOL = {"tool", "CPN Tools"};
    static constexpr ::std::pair<const char *, const char *> GENERATOR_VERSION = {"version", "4.0.1"};
    static constexpr ::std::pair<const char *, const char *> GENERATOR_FORMAT = {"format", "6"};
    static constexpr const char *CPNET = "cpnet";
    static constexpr const char *GLOBBOX = "globbox";
    static constexpr const char *BLOCK = "block";
    static constexpr const char *SD_BLOCK_NAME = "Standard declarations";
    static constexpr const char *OPTIONS = "options";
    static constexpr const char *BINDERS = "binders";
    static constexpr const char *INSTANCES = "instances";
    static constexpr const char *MONITOR_BLOCK = "monitorblock";
    static constexpr ::std::pair<const char *, const char *> MONITOR_BLOCK_NAME = {"name", "Monitors"};
    static constexpr const char *INDEX_NODE = "IndexNode";
    static constexpr ::std::pair<const char *, const char *> INDEX_NODE_EXPANDED = {"expanded", "false"};

    // General component attributes
    static constexpr const char *ID = "id";
    static constexpr const char *NAME = "name";

    // Page attribs
    static constexpr const char *PAGE = "page";
    static constexpr const char *PAGEATTR = "pageattr";

    // Color Set
    static constexpr const char *COLOR = "color";
    static constexpr const char *LAYOUT = "layout";

    // Color set declarations
    static constexpr const char *UNIT = "unit";
    static constexpr const char *BOOL = "bool";
    static constexpr const char *INT = "int";
    static constexpr const char *INTINF = "intinf";
    static constexpr const char *TIME = "time";
    static constexpr const char *REAL = "real";
    static constexpr const char *STRING = "string";
    static constexpr const char *PRODUCT = "product";
    static constexpr const char *ALIAS = "alias";
};

_END_PSM_NM_