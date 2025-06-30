# std::hive


## https://eel.is/c++draft/hive.overview

### 1
> A hive is a type of sequence container that provides constant - time insertion and erasure operations.
Storage is automatically managed in multiple memory blocks, referred to as element blocks.
Insertion position is determined by the container, and insertion may re - use the memory locations of erased elements.

### 2
>Element blocks which contain elements are referred to as active blocks, those which do not are referred to as reserved blocks.
Active blocks which become empty of elements are either deallocated or become reserved blocks.
Reserved blocks become active blocks when they are used to store elements.
A user can create additional reserved blocks by calling reserve.

### 3
>Erasures use unspecified techniques of constant time complexity to identify the memory locations of erased elements, 
which are subsequently skipped during iteration, as opposed to relocating subsequent elements during erasure.

### 4
>Active block capacities have an implementation - defined growth factor(which need not be integral), 
for example a new active block's capacity could be equal to the summed capacities of the pre-existing active blocks.

### 5
>Limits can be placed on both the minimum and maximum element capacities of element blocks, both by users and implementations.




## https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p0447r28.html#introduction

>The purpose of a container in the standard library cannot be to provide the optimal solution for all scenarios. 
Inevitably in fields such as high-performance trading or gaming, the optimal solution within critical loops will be a custom-made one that fits that scenario perfectly. 
However, outside of the most critical of hot paths, there is a wide range of application for more generalized solutions.

>Hive is a formalisation, extension and optimization of what is typically known as a 'bucket array' or 'object pool' container in game programming circles.
Thanks to all the people who've come forward in support of the paper over the years, I know that similar structures exist in various incarnations across many fields including high-performance computing, 
high performance trading, 3D simulation, physics simulation, robotics, server/client application and particle simulation fields 
(see this google groups discussion, the hive supporting paper #1 and appendix links to prior art).

>The concept of a bucket array is : 
you have multiple memory blocks of elements, and a boolean token for each element which denotes whether or not that element is 'active' or 'erased' - commonly known as a skipfield.
If it is 'erased', it is skipped over during iteration.
When all elements in a block are erased, the block is removed, so that iteration does not lose performance by having to skip empty blocks.
If an insertion occurs when all the blocks are full, a new memory block is allocated.

>The advantages of this structure are as follows : because a skipfield is used, no reallocation of elements is necessary upon erasure.
Because the structure uses multiple memory blocks, insertions to a full container also do not trigger reallocations.
This means that element memory locations stay stable and iterators stay valid regardless of erasure / insertion.
This is highly desirable, for example, in game programming because there are usually multiple elements in different containers which need to reference each other during gameplay, 
and elements are being inserted or erased in real time.
The only non - associative standard library container which also has this feature is std::list, but it is undesirable for performance and memory - usage reasons.
This does not stop it being used in many open - source projects due to this feature and its splice operations.

>Problematic aspects of a typical bucket array are that they tend to have a fixed memory block size, tend to not re - use memory locations from erased elements, 
and utilize a boolean skipfield.
The fixed block size(as opposed to block sizes with a growth factor) and lack of erased - element re - use leads to far more allocations / deallocations than is necessary,
and creates memory waste when memory blocks have many erased elements but are not entirely empty.
Given that allocation is a costly operation in most operating systems, this becomes important in performance - critical environments.
The boolean skipfield makes iteration time complexity at worst O(n) in capacity(), as there is no way of knowing ahead of time how many erased elements occur between any two non - erased elements.
This can create variable latency during iteration.
It also requires branching code for each skipfield node, which may cause performance issues on processors with deep pipelines and poor branch - prediction failure performance.

>A hive uses a non - boolean method for skipping erased elements, which allows for more - predictable iteration performance than a bucket array and O(1) iteration time complexity; 
the latter of which means it meets the C++ standard requirements for iterators, which a boolean method doesn't. 
It has an (optional - on by default) growth factor for memory blocks and reuses erased element locations upon insertion, which leads to fewer allocations/reallocations. 
Because it reuses erased element memory space, the exact location of insertion is undefined. 
Insertion is therefore considered unordered, but the container is sortable. 
Lastly, because there is no way of predicting in advance where erasures ('skips') may occur between non-erased elements, 
an O(1) time complexity [ ] operator is not possible and thereby the container is bidirectional but not random-access.

>There are two patterns for accessing stored elements in a hive : 
the first is to iterate over the container and process each element(or skip some elements using advance / prev / next / iterator++ / --functions).
The second is to store an iterator returned by insert() (or a pointer derived from the iterator) in some other structure and access the inserted element in that way.
To better understand how insertion and erasure work in a hive, see the following diagrams
