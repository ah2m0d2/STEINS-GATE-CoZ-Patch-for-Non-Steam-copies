#pragma once

namespace view {

class Page;

class IPageStack {
   public:
    virtual ~IPageStack() {}
    virtual void push(Page *page) = 0;
    virtual void pop() = 0;
    virtual Page *currentPage() = 0;
};

}  // namespace view