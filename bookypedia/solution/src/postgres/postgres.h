#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/author.h"
#include "../domain/book.h"
#include "../domain/book_tags.h"
#include "../app/unit_of_work.h"

namespace postgres {

  class AuthorRepositoryImpl : public domain::AuthorRepository {
  public:
    explicit AuthorRepositoryImpl(pqxx::work& work)
      : work_{ work } {
    }

    void Save(const domain::Author& author) override;
    void Delete(const domain::AuthorId& author_id) override;
    void Edit(const domain::AuthorId& author_id, const std::string& name) override;
    domain::Author GetAuthorById(const domain::AuthorId& author_id) override;
    std::optional<domain::Author> GetAuthorByName(const std::string& name) override;
    std::vector<domain::Author> GetAuthors() override;



  private:
    pqxx::work& work_;
  };



  class BookRepositoryImpl : public domain::BookRepository {

  public:

    explicit BookRepositoryImpl(pqxx::work& work)
      : work_{ work } {
    }

    void Save(const domain::Book& book) override;

    void Delete(const domain::BookId& book_id) override;

    void DeleteByAuthorId(const domain::AuthorId& author_id) override;

    void Edit(const domain::BookId book_id, const std::string& title,
      int publication_year) override;
    std::vector<domain::Book> GetBooks() override;

    std::vector<domain::Book> GetBooksByAuthor(const domain::AuthorId& author_id) override;

  private:

    

    pqxx::work& work_;

  };



  class BookTagRepositoryImpl : public domain::BookTagRepository {
  
  public:
    
    explicit BookTagRepositoryImpl(pqxx::work& work)
      : work_{ work } {}

    void Save(const domain::BookTag& book_tag) override;
    void Delete(const domain::BookId& book_id) override;
    std::vector<std::string> GetBookTags(const domain::BookId& book_id) const override;

  private:
    pqxx::work& work_;
  };


  class UnitOfWorkImpl : public app::UnitOfWork {
  public:
    explicit UnitOfWorkImpl(pqxx::connection& connection) :
      work_(connection),
      authors_(work_),
      books_(work_),
      bookTags_(work_) {}

    void Commit() override {
      work_.commit();
    }

    domain::AuthorRepository& Authors() override {
      return authors_;
    }

    domain::BookRepository& Books() override {
      return books_;
    }

    domain::BookTagRepository& BookTags() override {
      return bookTags_;
    }
  
  private:
    pqxx::work work_;
    AuthorRepositoryImpl authors_;
    BookRepositoryImpl books_;
    BookTagRepositoryImpl bookTags_;

  };

  class UnitOfWorkFactoryImpl : public app::UnitOfWorkFactory {
  
  public:
    
    explicit UnitOfWorkFactoryImpl(pqxx::connection& connection)
      : connection_(connection) {}

    std::unique_ptr<app::UnitOfWork> CreateUnitOfWork() override {
      return std::make_unique<UnitOfWorkImpl>(connection_);
    }

  private:

    pqxx::connection& connection_;
  };


  class Database {
  public:
    explicit Database(pqxx::connection connection);

    app::UnitOfWorkFactory& GetUnitOfWorkFactory() {
      return unit_of_work_factory_;
    }


  private:
    
    pqxx::connection connection_;

    UnitOfWorkFactoryImpl unit_of_work_factory_{connection_};

  };

}  // namespace postgres