#pragma once

#include "../domain/player_record.h"
#include "../app/unit_of_work.h"
#include "connection_pool.h"

#include <pqxx/connection>
#include <pqxx/transaction>


namespace db_postgres {

  class PlayerRecordRepositoryImpl : public db_domain::PlayerRecordRepository {
  
  public:

    explicit PlayerRecordRepositoryImpl(pqxx::work& work) : work_(work) {}

    void Save(const db_domain::PlayerRecord& record) override;

    void SavePlayersRecords(const std::vector<db_domain::PlayerRecord>& players_records) override;

    std::vector<db_domain::PlayerRecord> GetPlayersRecords(size_t offset, size_t limit) override;

  private:

    pqxx::work& work_;
  
  };

  class UnitOfWorkImpl : public db_app::UnitOfWork {
  public:
    explicit UnitOfWorkImpl(ConnectionPool::ConnectionWrapper&& connection) :
      connection_(std::move(connection)),
      work_(*connection_),
      players_records_(work_) {}

    void Commit() override {
      work_.commit();
    }

    db_domain::PlayerRecordRepository& PlayerRecords() override {

      return players_records_;
    }

  private:
    ConnectionPool::ConnectionWrapper connection_;
    pqxx::work work_;  
    PlayerRecordRepositoryImpl players_records_;

  };

  class UnitOfWorkFactoryImpl : public db_app::UnitOfWorkFactory {

  public:

    explicit UnitOfWorkFactoryImpl(ConnectionPool& connection_pool)
      : connection_pool_(connection_pool) {}

    std::unique_ptr<db_app::UnitOfWork> CreateUnitOfWork() override {
      return std::make_unique<UnitOfWorkImpl>(connection_pool_.GetConnection());
    }

  private:

    ConnectionPool& connection_pool_;
  };


  class Database {
  public:
    Database(size_t size_pool, std::string url);

    db_app::UnitOfWorkFactory& GetUnitOfWorkFactory() {
      return unit_of_work_factory_;
    }


  private:

    ConnectionPool connection_pool_;

    UnitOfWorkFactoryImpl unit_of_work_factory_{ connection_pool_ };

  };

}