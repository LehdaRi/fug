#ifndef FUG_TEST_RESOURCES_HPP
#define FUG_TEST_RESOURCES_HPP


#include "Resource.hpp"


namespace fug {

    class FUG_RESOURCE(TestResource1) {
    public:
        FUG_RESOURCE_INIT_DESTROY_DECL

        int a;
    };

}


#endif // FUG_TEST_RESOURCES_HPP
