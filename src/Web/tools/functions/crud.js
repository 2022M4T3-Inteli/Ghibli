// CRUD

// Create
function createNode(table, columns, values) {
    var sql = "INSERT INTO " + table + " (" + columns + ") VALUES (" + values + ")";
    console.log(sql);
    return sql;
}

// Read
function readNode(table, columns, condition) {
    if (condition == null) {
        var sql = "SELECT " + columns + " FROM " + table;
    } else {
    var sql = "SELECT " + columns + " FROM " + table + " WHERE " + condition;
    }
    return sql;
}

// Update
function updateNode(table, columns, values, condition) {
    var sql = "UPDATE " + table + " SET (" + columns + ") = " + "('" + values + "')" + " WHERE " + condition;
    console.log(sql);
    return sql;
}

// Delete
function deleteNode(table, condition) {
    var sql = "DELETE FROM " + table + " WHERE " + condition;
    return sql;
}

module.exports = { createNode, readNode, updateNode, deleteNode };
