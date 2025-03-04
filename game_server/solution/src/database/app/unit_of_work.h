#pragma once

#include <memory>

#include "../domain/player_record.h"

namespace db_app {

    struct UnitOfWork {

      virtual void Commit() = 0;

      virtual db_domain::PlayerRecordRepository& PlayerRecords() = 0;

      virtual ~UnitOfWork() = default;

    };

    class UnitOfWorkFactory {

    public:

      virtual std::unique_ptr<UnitOfWork> CreateUnitOfWork() = 0;

    protected:

      ~UnitOfWorkFactory() = default;

    };

}