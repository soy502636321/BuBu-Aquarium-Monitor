#include "router.h"
#include "vars.h"

// 单例
Router &Router::instance()
{
    static Router router;
    return router;
}

// 构造函数
Router::Router()
{
    top = 0;

    history[0].page_id = content_page_t_HOME_PAGE;   // 修改成你的首页枚举
    history[0].argument = nullptr;
}

// 进入页面
void Router::push(RouterPage_t page)
{
    if (top < MAX_HISTORY - 1)
    {
        top++;
    }
    //page.argument = reinterpret_cast<void *>(argument);
    history[top] = page;
    load(page);
}

// 返回
void Router::pop()
{
    if (top == 0)
        return;
    top--;
    load(history[top]);
}

// 替换当前页面
void Router::replace(RouterPage_t page)
{
    //page.argument = reinterpret_cast<void *>(argument);
    history[top] = page;
    load(page);
}

// 清空历史
void Router::clear()
{
    top = 0;

    history[0].page_id = content_page_t_HOME_PAGE;
    history[0].argument = nullptr;
}

// 当前页面
RouterPage_t Router::current() const
{
    return history[top];
}

// 当前参数
uintptr_t Router::argument() const
{
    return reinterpret_cast<uintptr_t>(history[top].argument);
}

// 是否可以返回
bool Router::canBack() const
{
    return top > 0;
}

// 真正加载页面
void Router::load(RouterPage_t page)
{
	set_var_content_page(page.page_id); 
}