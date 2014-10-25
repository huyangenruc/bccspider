#ifndef FILTER_H
#define FILTER_H

#include <iostream>

#include "bloom_filter.h"


class Filter{
public:
    bloom_filter* bf;
public:
    Filter(int maxUrl,double falseProbability){
        bloom_parameters parameters;

        // How many elements roughly do we expect to insert?
        parameters.projected_element_count = maxUrl;

        // Maximum tolerable false positive probability? (0,1)
        parameters.false_positive_probability = falseProbability; // 1 in 10000

        // Simple randomizer (optional)
        parameters.random_seed = 0xA5A5A5A5;

        if (!parameters)
        {
            std::cout << "Error - Invalid set of bloom filter parameters!" << std::endl;
           // return ;
        }
        parameters.compute_optimal_parameters();

        //Instantiate Bloom Filter
        //bloom_filter filter(parameters);
        this->bf = new bloom_filter(parameters);
    }

    ~Filter(){

    }

};


#endif // FILTER_H
