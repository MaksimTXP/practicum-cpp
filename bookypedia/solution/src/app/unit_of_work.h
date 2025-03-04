#pragma once

#include <memory>

#include "../domain/author_fwd.h"
#include "../domain/book_fwd.h"
#include "../domain/book_tags_fwd.h"

namespace app {
  
  struct UnitOfWork {
  
    virtual void Commit() = 0;
    virtual domain::AuthorRepository& Authors() = 0;
    virtual domain::BookRepository& Books() = 0;
    
    virtual domain::BookTagRepository& BookTags() = 0;
    virtual ~UnitOfWork() = default;
  
  };

  class UnitOfWorkFactory {
    
  public:

    virtual std::unique_ptr<UnitOfWork> CreateUnitOfWork() = 0;

  protected:

    ~UnitOfWorkFactory() = default;
  
  };

}