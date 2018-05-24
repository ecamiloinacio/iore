# IORE - The Parallel I/O and Storage Performance Evaluation Tool

The *IOR-Extended* (IORE) is a unified and flexible tool for performance evaluation of modern high-performance parallel I/O software stacks and storage systems. Although its core was designed after the popular [IOR benchmark](http://github.com/llnl/ior), IORE is more than a synthetic I/O workload generator, it focuses on meeting I/O research works requirements on complex and reproducible experimental workflows. 

Among the main new features introduced in IORE are:
* **Experiment-driven execution**: every execution of IORE is guided by an *experiment*. The experiment, is composed of one or more *runs*, which, in turn, contains a write or write+read *test* (currently, read-only tests are not supported). Experiments, runs, and tests have particular parameters that allows defining all aspects of the execution, including the number of replications of the experiment, run execution ordering, characteristics of the workload, to name a few.

* **Heterogeneous offset-based workloads**: IORE provides parameters for specifying offset-based workloads (*e.g.,* request and block size) with different sizes per process. This parameters can be set explicitly or randomly generated, using user-defined statistical distributions. This way, heterogeneous I/O workloads can be reproduced and their performance evaluated.

* **Dataset-based workloads**: focusing precisely reproducing typical workloads on computational science applications, IORE introduces a new set of parameters specific for generating dataset-based workloads. This feature makes easier for users to reproduce, for instance, an application writing or reading a Cartesian dataset distributed across multiple processes.

* **Storage system integration**: a demand observed in several previous research works refers to I/O experiments with different storage system parameters, such as the stripe size in parallel file systems. Such experiments can be handled by IORE through an *Abstract File Storage Backend* (AFSB) implementation. This extensible proposal aims at integrating the storage system configuration into the experimental workflow, making it easier for coordinating and conducting such experiments.

* **Performance statistics export**: IORE includes a features for exporting collected performance metrics to files after experiment completion. Currently, CSV format are adopted, allowing users to easily analyze their results in a great number of data analysis tools.

A manual describing all available parameters will be made available soon. For more details on the IORE design and for reference when citing IORE, we refer to the following paper (to appear) in the ISCC '18:

Inacio, E.C., Dantas, M.A.R. (2018). *IORE: A Flexible and Distributed I/O Performance Evaluation Tool for Hyperscale Storage Systems*. In.: Proceedings of the IEEE International Symposium on Computers and Communications.

## How to build?

In order to compile IORE, you will need to have an MPI implementation installed in your system.

~~~
$ tar xzf iore-[VERSION].tgz
$ cd iore
$ make
~~~

The executable will be placed in the `iore/bin` directory.

## How to use?

For a local, single process execution:
~~~
$ iore
~~~

For a distributed, multi-process execution:
~~~
$ mpiexec -n [NUMBER OF PROCESSES] -f [HOSTS] iore
~~~

You may have to include `iore/bin` directory in your PATH in order to refer to `iore` directly. Otherwise, refer to the absolute path of the `iore` executable.

## Command-line options

In the current state of development, IORE has two command-line options:
* *`-v`*: verbose output; includes summary with experiment, experiment replica, and run repetition execution times.
* *`-f FILE`*: pass to IORE the experiment definition file; the `examples` directory in the IORE package provide some sample experiment definitions.

## Remarks

IORE is a work-in-progress. Although it is not supposed to cause any damage to your system, use it at your own risk.
