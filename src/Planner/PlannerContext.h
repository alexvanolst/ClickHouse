#pragma once

#include <Common/HashTable/Hash.h>

#include <Core/Names.h>
#include <Core/NamesAndTypes.h>

#include <Interpreters/Context_fwd.h>
#include <Interpreters/SubqueryForSet.h>
#include <Interpreters/Set.h>

#include <Analyzer/IQueryTreeNode.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int LOGICAL_ERROR;
}

using ColumnIdentifier = std::string;

class TableExpressionColumns
{
public:
    using ColumnNameToColumnIdentifier = std::unordered_map<std::string, ColumnIdentifier>;

    bool hasColumn(const std::string & column_name) const
    {
        return alias_columns_names.contains(column_name) || columns_names.contains(column_name);
    }

    void addColumn(const NameAndTypePair & column, const ColumnIdentifier & column_identifier)
    {
        if (hasColumn(column.name))
            throw Exception(ErrorCodes::LOGICAL_ERROR, "Column with name {} already exists");

        columns_names.insert(column.name);
        columns.push_back(column);
        column_name_to_column_identifier.emplace(column.name, column_identifier);
    }

    void addColumnIfNotExists(const NameAndTypePair & column, const ColumnIdentifier & column_identifier)
    {
        if (hasColumn(column.name))
            return;

        columns_names.insert(column.name);
        columns.push_back(column);
        column_name_to_column_identifier.emplace(column.name, column_identifier);
    }

    void addAliasColumnName(const std::string & column_name)
    {
        alias_columns_names.insert(column_name);
    }

    const NameSet & getAliasColumnsNames() const
    {
        return alias_columns_names;
    }

    const NameSet & getColumnsNames() const
    {
        return columns_names;
    }

    const NamesAndTypesList & getColumns() const
    {
        return columns;
    }

    const ColumnNameToColumnIdentifier & getColumnNameToIdentifier() const
    {
        return column_name_to_column_identifier;
    }

    const ColumnIdentifier & getColumnIdentifierOrThrow(const std::string & column_name) const
    {
        auto it = column_name_to_column_identifier.find(column_name);
        if (it == column_name_to_column_identifier.end())
            throw Exception(ErrorCodes::LOGICAL_ERROR,
                "Column identifier for name {} does not exists",
                column_name);

        return it->second;
    }

    const ColumnIdentifier * getColumnIdentifierOrNull(const std::string & column_name) const
    {
        auto it = column_name_to_column_identifier.find(column_name);
        if (it == column_name_to_column_identifier.end())
            return nullptr;

        return &it->second;
    }

private:
    /// Valid for table, table function, query table expression nodes
    NamesAndTypesList columns;

    /// Valid for table, table function, query table expression nodes
    NameSet columns_names;

    /// Valid only for table table expression node
    NameSet alias_columns_names;

    /// Valid for table, table function, query table expression nodes
    ColumnNameToColumnIdentifier column_name_to_column_identifier;
};

/// Subquery node for set
struct SubqueryNodeForSet
{
    QueryTreeNodePtr subquery_node;
    SetPtr set;
};

/** Global planner context contains common objects that are shared between each planner context.
  *
  * 1. Prepared sets.
  * 2. Subqueries for sets.
  */
class GlobalPlannerContext
{
public:
    GlobalPlannerContext() = default;

    using SetKey = std::string;
    using SetKeyToSet = std::unordered_map<String, SetPtr>;
    using SetKeyToSubqueryNode = std::unordered_map<String, SubqueryNodeForSet>;

    /// Get set key for query node
    SetKey getSetKey(const QueryTreeNodePtr & set_source_node) const;

    /// Register set for set key
    void registerSet(const SetKey & key, SetPtr set);

    /// Get set for key, if no set is registered null is returned
    SetPtr getSetOrNull(const SetKey & key) const;

    /// Get set for key, if no set is registered logical exception is throwed
    SetPtr getSetOrThrow(const SetKey & key) const;

    /** Register subquery node for set
      * Subquery node for set node must have QUERY or UNION type and set must be initialized.
      */
    void registerSubqueryNodeForSet(const SetKey & key, SubqueryNodeForSet subquery_node_for_set);

    /// Get subquery nodes for sets
    const SetKeyToSubqueryNode & getSubqueryNodesForSets() const
    {
        return set_key_to_subquery_node;
    }
private:
    SetKeyToSet set_key_to_set;

    SetKeyToSubqueryNode set_key_to_subquery_node;
};

using GlobalPlannerContextPtr = std::shared_ptr<GlobalPlannerContext>;

class PlannerContext
{
public:
    PlannerContext(ContextPtr query_context_, GlobalPlannerContextPtr global_planner_context_);

    const ContextPtr & getQueryContext() const
    {
        return query_context;
    }

    const GlobalPlannerContextPtr & getGlobalPlannerContext() const
    {
        return global_planner_context;
    }

    GlobalPlannerContextPtr & getGlobalPlannerContext()
    {
        return global_planner_context;
    }

    const std::unordered_map<QueryTreeNodePtr, TableExpressionColumns> & getTableExpressionNodeToColumns() const
    {
        return table_expression_node_to_columns;
    }

    std::unordered_map<QueryTreeNodePtr, TableExpressionColumns> & getTableExpressionNodeToColumns()
    {
        return table_expression_node_to_columns;
    }

    ColumnIdentifier getColumnUniqueIdentifier(const QueryTreeNodePtr & column_source_node, std::string column_name = {});

    void registerColumnNode(const QueryTreeNodePtr & column_node, const ColumnIdentifier & column_identifier);

    const ColumnIdentifier & getColumnNodeIdentifierOrThrow(const QueryTreeNodePtr & column_node) const;

    const ColumnIdentifier * getColumnNodeIdentifierOrNull(const QueryTreeNodePtr & column_node) const;

private:
    /// Query context
    ContextPtr query_context;

    /// Global planner context
    GlobalPlannerContextPtr global_planner_context;

    /// Column node to column identifier
    std::unordered_map<QueryTreeNodePtr, ColumnIdentifier> column_node_to_column_identifier;

    /// Table expression node to columns
    std::unordered_map<QueryTreeNodePtr, TableExpressionColumns> table_expression_node_to_columns;

    size_t column_identifier_counter = 0;
};

using PlannerContextPtr = std::shared_ptr<PlannerContext>;

}
