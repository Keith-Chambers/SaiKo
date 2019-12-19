#ifndef DATASOURCE_H
#define DATASOURCE_H

namespace kpl
{
    class DataSource
    {
    public:
        virtual void *get() = 0;
    };
}

#endif // DATASOURCE_H
