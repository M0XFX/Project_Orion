#ifndef DECIMATOR_H
#define DECIMATOR_H

#include "IQBuffer.h"

namespace HFSDR
{

class Decimator
{
public:
    Decimator();

    void setFactor(int factor);
    int factor() const;

    void process(const IQBuffer& input, IQBuffer& output);

private:
    int m_factor = 8;
};

} // namespace HFSDR

#endif
