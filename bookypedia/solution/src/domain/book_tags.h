#pragma once

#include <string>

#include "book.h"
#include "../util/tagged_uuid.h"

namespace domain {

  class BookTag {
  
  public:
    BookTag (BookId book_id, std::string tag)
      : book_id_(std::move(book_id))
      , tag_(std::move(tag)) {}

    const BookId& GetBookId() const noexcept {
      return book_id_;
    }

    const std::string GetTag() const noexcept {
      return tag_;
    }

  private:

    BookId book_id_;

    std::string tag_;

  };

  class BookTagRepository {
  
  public:

    virtual void Save(const BookTag& book_tag) = 0;
    virtual void Delete(const BookId& book_id) = 0;
    virtual std::vector<std::string> GetBookTags(const BookId& book_id) const = 0;
    

  protected:
    ~BookTagRepository() = default;

  };

}