Code for generating instances used for computational experiments on ["The price-elastic knapsack problem"](https://doi.org/10.1016/j.omega.2023.103003).

### Compilation

#### Ubuntu

```sh
sudo apt install ligpugixml-dev
g++ -o instance-generator main.cpp -lpugixml
```

### Usage
```sh
 .\instance-generator -help
Usage:
-n <size of instance>
-m <number of bins>
-seed <random seed>
-numinstances <how many random instance should be generated>
-fixedcosts <fixed cost type>
         0: None
         1: Only costs
         2: Costs and profits
-pricelimit <price limit behaviour>
         0: None
         1: Individual price limit
         2: Single price limit
-weight <>
         0: Polynomial (please specify degree)
         1: Piecewise Linear (please specify number of pieces)
         2: Piecewise Polynomial (please specify degree and number of pieces)
-degree <weight function degree for the polynomial and piecewise polynomial case>
-numpieces <weight function number of pieces for the piecewise and piecewise polynomial case>
-dataset <desired dataset name>
```

### Paper Instances

Unfortunately, the exact instances used for the computational experiments in the paper were lost.

A new set of instances, as defined in the paper, can be generated using:

```sh
for n in 50, 100, 500, 1000, 2000; do

# Section 2.2: Affine-Linear Instances
./instances-generator \
    -dataset linear_single_price_limit \
    -n $n \
    -m 1 \
    -numinstances 100 \
    -fixedcosts 2 \
    -pricelimit 2 \
    -weight 0 \
    -degree 1

# Section 2.3: Piece-Linear Instances
./instances-generator \
    -dataset piecewise_linear_single_price_limit \
    -n $n \
    -m 1 \
    -numinstances 100 \
    -fixedcosts 2 \
    -pricelimit 2 \
    -weight 1 \
    -numpieces 10
done
```

A sample set of instances can also be found in the `instances` folder.
