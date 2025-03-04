#pragma once

#include <string>
#include <vector>

#include "author.h"
#include "../util/tagged_uuid.h"

namespace domain {

  namespace detail {
    struct BookTag {};
  }// namespace detail

  using BookId = util::TaggedUUID<detail::BookTag>;

  class Book {

  public:

    Book(BookId id, 
      AuthorId author_id, 
      std::string title, 
      int publication_year) :
      id_(id), 
      author_id_(author_id), 
      title_(title), 
      publication_year_(publication_year)
    {}

    const BookId& GetId() const noexcept {
      return id_;
    }

    const AuthorId& GetAuthorId() const noexcept {
      return author_id_;
    }

    const std::string& GetTitle() const noexcept {
      return title_;
    }

    int GetPublicationYear() const noexcept {
      return publication_year_;
    }

    void EditTitle(const std::string& new_title) {
      title_ = new_title;
    }

    void EditPublicationYear(int new_publication_year) {
      publication_year_ = new_publication_year;
    }
 

  private:

    BookId id_;
    AuthorId author_id_;
    std::string title_;
    int publication_year_;
 

  };

  class BookRepository {
  
  public:

    virtual void Save(const Book& book) = 0;

    virtual void Delete(const BookId& book_id) = 0;

    virtual void DeleteByAuthorId(const AuthorId& author_id) = 0;
    virtual void Edit(const BookId book_id, const std::string& title,
      int publication_year) = 0;

    virtual std::vector<Book> GetBooks() = 0;

    virtual std::vector<Book> GetBooksByAuthor(const AuthorId& author_id) = 0;


  protected:

    ~BookRepository() = default;

  };

}// namespace detail