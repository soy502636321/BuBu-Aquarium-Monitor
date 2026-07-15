#include "stdint.h"
#include "vars.h" 


typedef struct {
	content_page_t page_id;
	void *argument;
} RouterPage_t ;

class Router
{
public:

    static Router &instance();

    void push(RouterPage_t page);

    void pop();

    void replace(RouterPage_t page);

    void clear();

    RouterPage_t current() const;

    uintptr_t argument() const;

    bool canBack() const;

private:

    Router();

    void load(RouterPage_t page);

private:

    static constexpr uint32_t MAX_HISTORY = 10;

    RouterPage_t history[MAX_HISTORY];

    uint32_t top;
};

