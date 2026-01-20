#include "Table.h"

FieldSpec::FieldSpec(std::string name, bool foreign, bool primary, const DataType& type)
: name(std::move(name)), foreign(foreign), primary(primary), type(type) {}

FieldSpec::FieldSpec(const std::string& name, const DataType& type)
: FieldSpec(name, false, false, type) {}

void Table::Column::ensureValid(const std::any& e) const
{
    if (!spec.type.is_assignable(e))
    {
        throw std::runtime_error("Provided element has invalid type.");
    }

    if (spec.primary)
    {
        for (const std::any& o : data)
        {
            if (spec.type.equal(o, e))
            {
                throw std::runtime_error("Primary key already exists.");
            }
        }
    }
}

Table::Table(std::initializer_list<FieldSpec> specs) : _size(0)
{
    columns.reserve(specs.size());

    std::size_t i = 0;
    for (const FieldSpec& spec : specs)
    {
        if (spec.primary)
        {
            if (primaryIndex.has_value())
            {
                throw std::runtime_error("Only one column can be marked as a primary key.");
            }

            primaryIndex = i;
        }

        indices.emplace(spec.name, columns.size());
        columns.emplace_back(spec);
        ++i;
    }
}

std::size_t Table::size() const
{
    return _size;
}

const std::vector<std::any>& Table::column(const std::string& name) const
{
    return columns[indices.at(name)].data;
}

std::optional<std::size_t> Table::find_index(const Column& column, const std::any& key)
{
    auto it = std::ranges::find_if(column.data, [&](const std::any& value)
    {
        return column.spec.type.equal(key, value);
    });

    if (it == column.data.end())
    {
        return std::nullopt;
    }

    return std::distance(column.data.begin(), it);
}

std::optional<std::size_t> Table::find_index(const std::any& primaryKey) const
{
    if (!primaryIndex.has_value())
    {
        throw std::runtime_error("This table does not have a primary property.");
    }

    return find_index(columns[primaryIndex.value()], primaryKey);
}

bool Table::contains(const std::string& name, const std::any& key) const
{
    return find_index(columns[indices.at(name)], key).has_value();
}

bool Table::contains(const std::any& primaryKey) const
{
    return find_index(primaryKey).has_value();
}

void Table::removeRow(std::size_t rowIndex)
{
    for (auto& [_, data] : columns)
    {
        data[rowIndex] = std::move(data.back());
        data.pop_back();
    }

    --_size;
}

void Table::remove(const std::any& primaryKey)
{
    auto index = find_index(primaryKey);
    if (index.has_value())
    {
        removeRow(*index);
    }
}
