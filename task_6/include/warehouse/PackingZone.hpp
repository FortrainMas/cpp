#include <memory>
#include <vector>

#include "entities/Table.hpp"

class PackingZone {
    private:
        std::vector<std::shared_ptr<Table>> tables;
    
    public:
        PackingZone(int num_tables) {
            for(int i = 0; i < num_tables; i++){
                tables.push_back(std::make_shared<Table>(3, 3));
            }
        }

        std::weak_ptr<Table> getTable(int table_id) {
            return tables[table_id];
        }
};

