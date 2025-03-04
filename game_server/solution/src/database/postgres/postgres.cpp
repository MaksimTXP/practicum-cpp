#include "postgres.h"
#include <pqxx/pqxx>
#include "../../constants/config.h"



namespace db_postgres {

  using pqxx::operator"" _zv;
  
  void PlayerRecordRepositoryImpl::Save(const db_domain::PlayerRecord& record) {
    work_.exec_params(
      R"( 
         INSERT INTO retired_players (name, score, play_time)
         VALUES ($1, $2, $3);)"_zv,
         record.GetName(), record.GetScore(), record.GetPlayTime().count()
    
    );
  
  }

  void PlayerRecordRepositoryImpl::SavePlayersRecords(const std::vector<db_domain::PlayerRecord>& players_records) {
    for (const db_domain::PlayerRecord& record : players_records) {
      Save(record);
    }
  }

  std::vector<db_domain::PlayerRecord> PlayerRecordRepositoryImpl::GetPlayersRecords(size_t offset, size_t limit) {

    if (limit > config::DataBase::LIMIT_RECORDS) {
      throw std::runtime_error("The number of records has been exceeded");
    }
    const auto query_text =
      "SELECT name, score, play_time "
      "FROM retired_players "
      "ORDER BY score DESC, play_time, name "
      "LIMIT " + std::to_string(limit) +
      " OFFSET " + std::to_string(offset) + ";";

    std::vector<db_domain::PlayerRecord> result;
    for (auto [name, score, play_time] :
      work_.query<std::string, size_t, int64_t>(query_text)) {
      result.emplace_back(db_domain::PlayerRecord{
          name,
          score,
          std::chrono::milliseconds(play_time),
        });
    }

    return result;
  }
   Database::Database(size_t size_pool, std::string url):connection_pool_(size_pool, [url]() {
    return std::make_shared<pqxx::connection>(url);
    }) {
    auto connection = connection_pool_.GetConnection();
    pqxx::work work_{ *connection };
    work_.exec(R"(
        CREATE TABLE IF NOT EXISTS retired_players (
            id SERIAL PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            score integer CONSTRAINT score_positive CHECK (score >= 0),
            play_time integer NOT NULL CONSTRAINT play_time_positive CHECK (play_time >= 0) 
        );
       CREATE INDEX IF NOT EXISTS retired_players_score ON retired_players (score); 
    )"_zv);
    work_.commit();
  
  }


}