#ifndef ENCODER_MATERIALPARAMETER_H_
#define ENCODER_MATERIALPARAMETER_H_

#include "Object.h"

namespace gplayencoder
{

class MaterialParameter : public Object
{
public:

    /**
     * Constructor.
     */
    MaterialParameter(void);

    /**
     * Destructor.
     */
    virtual ~MaterialParameter(void);

    virtual unsigned int getTypeId(void) const;
    virtual const char* getElementName(void) const;
    virtual void writeBinary(FILE* file);
    virtual void writeText(FILE* file);

private:
    std::vector<float> _value;
    unsigned int _type;
};

}

#endif
