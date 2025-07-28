#include <iostream>
#include <string>
#include <libpq-fe.h> // PostgreSQL C API header

// Function to handle connection and query execution
void execute_postgresql_command(const std::string& conninfo, const std::string& query) {
    // Establish a connection to the PostgreSQL database
    // conninfo example: "host=localhost port=5432 dbname=my_new_database user=my_app_user password=strong_password"
    PGconn *conn = PQconnectdb(conninfo.c_str());

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn); // Close the connection
        return;
    }

    std::cout << "Successfully connected to PostgreSQL database!" << std::endl;

    // Execute the SQL query
    PGresult *res = PQexec(conn, query.c_str());

    // Check if the query execution was successful
    if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Query execution failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);   // Clear the result object
        PQfinish(conn); // Close the connection
        return;
    }

    std::cout << "\nQuery executed successfully: \"" << query << "\"" << std::endl;

    // Process and display the query results (if any)
    int num_rows = PQntuples(res);    // Number of rows (tuples)
    int num_cols = PQnfields(res);    // Number of columns (fields)

    if (num_rows > 0 || num_cols > 0) { // Only print header if there are actual results
        // Print column headers
        for (int i = 0; i < num_cols; ++i) {
            std::cout << PQfname(res, i) << "\t\t"; // Get column name
        }
        std::cout << std::endl;

        // Print row data
        for (int i = 0; i < num_rows; ++i) {
            for (int j = 0; j < num_cols; ++j) {
                std::cout << PQgetvalue(res, i, j) << "\t\t"; // Get cell value
            }
            std::cout << std::endl;
        }
    } else {
        // For commands like INSERT, UPDATE, DELETE, PQcmdStatus gives details
        std::cout << "Command status: " << PQcmdStatus(res) << std::endl;
        std::cout << "Rows affected: " << PQcmdTuples(res) << std::endl;
    }

    // Clean up: clear the result object and close the connection
    PQclear(res);
    PQfinish(conn);
    std::cout << "\nDisconnected from PostgreSQL." << std::endl;
}

int main() {
    // IMPORTANT: Replace these with your actual PostgreSQL connection details
    // Refer to the 'postgresql_mac_setup' immersive for default user/database names
    // or the ones you created.
    std::string host = "localhost";
    std::string port = "5432"; // Default PostgreSQL port
    std::string dbname = "debdb"; // Or your macOS username, e.g., "deb"
    std::string user = "deb";     // Or your macOS username
    std::string password = "ubuntu"; // Or your actual password

    // Construct the connection string
    std::string conninfo = "host=" + host +
                           " port=" + port +
                           " dbname=" + dbname +
                           " user=" + user +
                           " password=" + password;

    // --- Example 1: Select data from the 'products' table ---
    std::cout << "--- Executing SELECT query ---" << std::endl;
    execute_postgresql_command(conninfo, "SELECT id, name, price FROM products;");

    std::cout << "\n=========================================\n";

    // --- Example 2: Insert new data (demonstrates PGRES_COMMAND_OK status) ---
    std::cout << "--- Executing INSERT query ---" << std::endl;
    execute_postgresql_command(conninfo, "INSERT INTO products (name, price) VALUES ('Keyboard', 75.00);");

    std::cout << "\n=========================================\n";

    // --- Example 3: Select all data again to see the inserted row ---
    std::cout << "--- Executing SELECT * query after insert ---" << std::endl;
    execute_postgresql_command(conninfo, "SELECT * FROM products;");

    std::cout << "\n=========================================\n";

    // --- Example 4: Querying PostgreSQL version (simple system query) ---
    std::cout << "--- Executing SELECT version() query ---" << std::endl;
    execute_postgresql_command(conninfo, "SELECT version();");

    return 0;
}

