# std::hive


## https://eel.is/c++draft/hive.overview

### 1
>> A hive is a type of sequence container that provides constant - time insertion and erasure operations.
Storage is automatically managed in multiple memory blocks, referred to as element blocks.
Insertion position is determined by the container, and insertion may re - use the memory locations of erased elements.
>
> hive 는 상수 시간의 삽입/삭제 연산을 제공하는 시퀀스 컨테이너의 일종입니다.
저장공간은 element blocks(원소 블록) 라고 불리는 여러개의 메모리 블록을 이용하여 자동으로 관리됩니다.
삽입 위치는 컨테이너에 의해 결정되고, 삭제된 원소의 메모리 위치를 재사용 할 수도 있습니다.


### 2
>> Element blocks which contain elements are referred to as active blocks, those which do not are referred to as reserved blocks.
Active blocks which become empty of elements are either deallocated or become reserved blocks.
Reserved blocks become active blocks when they are used to store elements.
A user can create additional reserved blocks by calling reserve.
>
> 원소를 포함하는 원소 블록은 active blocks(활성 블록) 라고 부르고, 그렇지 않은 경우에는 reserved blocks(예약된 블록) 라고 부릅니다.
비어있게 된 활성 블록은 메모리가 헤제(deallocated)되거나 예약된 블록이 될 수 있습니다.
원소를 저장하게 된 예약된 블록은 활성 블록이 됩니다.
사용자는 reserve 를 호출하여 추가적인 예약된 블록을 만들 수 있습니다.


### 3
>> Erasures use unspecified techniques of constant time complexity to identify the memory locations of erased elements, 
which are subsequently skipped during iteration, as opposed to relocating subsequent elements during erasure.
>
> Erasures 는 제거된 원소의 메모리 위치를 상수 시간에 식별하기 위해 정해지지 않은 기술을 사용합니다.(구현 방법이 정해지지 않았다는 의미)
원소 삭제중에 후속된 원소들을 재배치 하는 대신에, 순회하는 동안에는 건너뛰게됩니다.
(vector 처럼 원소 삭제후에 뒤의 원소들을 앞으로 재배치하지 않고, 그냥 순회할때 제거된 원소는 그냥 건너뛴다는 의미)


### 4
>> Active block capacities have an implementation - defined growth factor(which need not be integral), 
for example a new active block's capacity could be equal to the summed capacities of the pre-existing active blocks.
>
> 활성 블록의 capacity는 grow factor 로 구현이 정의됩니다.(정수일 필요는 없습니다.)
예를들어, 새로운 활성 블록의 capacity는 이미 존재하는 활성 블록들의 크기의 합과 동일할 수 있습니다.
(MSVC 에서 std::vector 에 대한 크기를 재할당할때 기존의 1.5 배를 할당하는 것을 생각하면 됨)


### 5
>> Limits can be placed on both the minimum and maximum element capacities of element blocks, both by users and implementations.
>
> 원소 블록의 capacity의 제한은 사용자 혹은 구현에서 최대/최소값을 정할 수 있습니다.



## https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p0447r28.html#introduction

### I. Introduction

>> The purpose of a container in the standard library cannot be to provide the optimal solution for all scenarios. 
Inevitably in fields such as high-performance trading or gaming, the optimal solution within critical loops will be a custom-made one that fits that scenario perfectly. 
However, outside of the most critical of hot paths, there is a wide range of application for more generalized solutions.
>
> 스탠다드 라이브러리의 컨테이너들은 모든 상황에서 최적의 솔루션을 제공할 수 없습니다.
필연적으로 금융거래 혹은 게이밍 등의 고성능이 필요한 현장에서, 핵심 루프 안에서의 최적 솔루션은 해당 시나리오에 맞는 직접 제작한 맞춤형 솔루션일 것입니다.
그러나, 가장 중요한 'hot paths' 밖에서는 더 일반화된 솔루션이 더 광범위하게 사용됩니다.
(hot path : 게임으로 치면 매 프레임 실행될 핵심 코드로, 최적화가 더 요구되는 코드 경로를 말함)

>> Hive is a formalisation, extension and optimization of what is typically known as a 'bucket array' or 'object pool' container in game programming circles.
Thanks to all the people who've come forward in support of the paper over the years, I know that similar structures exist in various incarnations across many fields including high-performance computing, 
high performance trading, 3D simulation, physics simulation, robotics, server/client application and particle simulation fields 
(see this google groups discussion, the hive supporting paper #1 and appendix links to prior art).
>
> Hive 는 게임 프로그래밍 분야에서 흔히 알려진 'bucket array' 혹은 'object pool' 의 정형화된, 확장된, 그리고 최적화된 컨테이너입니다.
수년에 걸쳐 이 논문을 지원해주신 분들 덕분에 고성능 컴퓨팅, 고성능 금융거래, 3D 시뮬레이션, 물리 시뮬레이션, 로보틱스, 서버/클라이언트 어플리케이션,
그리고 파티클 시뮬레이션 등을 포함한 많은 현장에서 이 hive 와 비슷한 구조가 다양한 형태로 사용됨들 알게 되었습니다.

>> The concept of a bucket array is : 
you have multiple memory blocks of elements, and a boolean token for each element which denotes whether or not that element is 'active' or 'erased' - commonly known as a skipfield.
If it is 'erased', it is skipped over during iteration.
When all elements in a block are erased, the block is removed, so that iteration does not lose performance by having to skip empty blocks.
If an insertion occurs when all the blocks are full, a new memory block is allocated.
>
> bucket array 의 개념은 이렇습니다 :
원소들에 대한 여러개의 메모리 블록이 존재하고, 각 원소마다 해당 원소가 '활성' 인지 '삭제됨' 인지를 표시하는 boolean 토큰이 있습니다. - 일반적으로 skipfield 라고 부릅니다.
만약 '삭제됨' 상태라면, 순회(iteration) 도중에 건너 뛰게됩니다.
만약 블록 안의 모든 원소가 '삭제됨' 이라면, 그 블록은 삭제되고, 순회시에 빈 블록을 건너뛰는 성능저하가 일어나지 않게 됩니다.
만약 모든 블록이 가득 찬 상태에서 삽입이 발생하면, 새로운 메모리 블록이 할당됩니다.

>> The advantages of this structure are as follows : because a skipfield is used, no reallocation of elements is necessary upon erasure.
Because the structure uses multiple memory blocks, insertions to a full container also do not trigger reallocations.
This means that element memory locations stay stable and iterators stay valid regardless of erasure / insertion.
This is highly desirable, for example, in game programming because there are usually multiple elements in different containers which need to reference each other during gameplay, 
and elements are being inserted or erased in real time.
The only non - associative standard library container which also has this feature is std::list, but it is undesirable for performance and memory - usage reasons.
This does not stop it being used in many open - source projects due to this feature and its splice operations.
>
> 이런 구조의 장점은 다음과 같습니다: skipfield 가 사용되기 때문에, 삭제시에 원소의 재배치가 필요하지 않습니다.
이 구조는 여러개의 메모리 블록을 사용하기 때문에, 꽉 찬 컨테이너에 삽입이 일어날 때, 재할당(혹은 재배치?)도 일어나지 않습니다.
이것은 원소의 메모리 위치가 안정적이고, 삽입/삭제와 관련없이 반복자가 유효함을 의미합니다.
이것은 매우 유용한데, 예를들어, 게임 프로그래밍에서 서로 다른 컨테이너에 저장된 여러개의 원소들이 게임플레이 동안 서로를 참조해야하면서, 원소의 삽입과 삭제가 실시간으로 일어나기 때문입니다.
이런 기능이 힜는 유일한 연관 컨테이너가 아닌 STL 의 컨테이너는 std::list 만이 존재하는데, 이것은 성능과 메모리 사용 측면에서 바람직하지 않습니다.
그러나 이것은(std::list) 위 특징과 splice 연산을 이유로 많은 오픈소스 프로젝트에서 계속 사용될 것입니다.

>> Problematic aspects of a typical bucket array are that they tend to have a fixed memory block size, tend to not re - use memory locations from erased elements, 
and utilize a boolean skipfield.
The fixed block size(as opposed to block sizes with a growth factor) and lack of erased - element re - use leads to far more allocations / deallocations than is necessary,
and creates memory waste when memory blocks have many erased elements but are not entirely empty.
Given that allocation is a costly operation in most operating systems, this becomes important in performance - critical environments.
The boolean skipfield makes iteration time complexity at worst O(n) in capacity(), as there is no way of knowing ahead of time how many erased elements occur between any two non - erased elements.
This can create variable latency during iteration.
It also requires branching code for each skipfield node, which may cause performance issues on processors with deep pipelines and poor branch - prediction failure performance.
>
> 일반적인 bucket array 의 문제점은 고정된 메모리 블록 크기를 가지고있고, 삭제된 원소의 메모리 위치를 재사용하지 않으며, boolean 의 skipfield 를 활용한다는 것입니다.
고정된 블록 크기(growth factor 를 사용하는 블록 크기와 반대로)와 삭제된 원소의 재활용의 부재는 더 많은 메모리 할당/해제를 불러오고, 메모리블록이 비어있지는 않지만 빈 공간이 많을 때의 메모리 낭비를 만듭니다.
메모리 할당이 대부분의 OS 에서 비용이 많이 드는 작업임을 감안할 때, 이것은 성능이 중요한 환경에서 중대한 문제입니다.
boolean 의 skipfield 는 순회 시간을 최악의 경우 O(n)으로 만드는 데 (n = capacity()), 2개의 삭제되지 않은 요소 사이에 얼마나 많은 원소가 삭제되었는지 미리 알수 있는 방법이 없기 때문입니다.
이것은 컨테이너의 순회동안에 가변적인 지연을 만들 수 있습니다.
또한 각각의 skipfiled 노드마다 분기코드를 필요로 하고, 이는 파이프라인 단계가 깊고 분기 예측 실패시 성능이 구린 프로세서에서 성능 문제를 일으킬 수 있습니다.

>> A hive uses a non - boolean method for skipping erased elements, which allows for more - predictable iteration performance than a bucket array and O(1) iteration time complexity; 
the latter of which means it meets the C++ standard requirements for iterators, which a boolean method doesn't. 
It has an (optional - on by default) growth factor for memory blocks and reuses erased element locations upon insertion, which leads to fewer allocations/reallocations. 
Because it reuses erased element memory space, the exact location of insertion is undefined. 
Insertion is therefore considered unordered, but the container is sortable. 
Lastly, because there is no way of predicting in advance where erasures ('skips') may occur between non-erased elements, 
an O(1) time complexity [ ] operator is not possible and thereby the container is bidirectional but not random-access.
>
> hive 는 삭제된 원소를 건너뛸 때 boolean 이 아닌 방법을 사용합니다. 이는 bucket array 보다 더 예측 가능한 순회 성능을 보여주고, O(1)의 시간 복잡도를 갖습니다(\+\+it/--it 의 성능이 O(1) 임을 의미하는듯 함). 
후자(O(1)의 순회성능) 은 boolean 방식은 만족할 수 없는 반복자에 대한 C\+\+ 표준 요구사항을 만족함을 의미합니다.
hive는 메모리 블록을 위한 grow factor(선택사항 - 기본적으로는 켜짐)을 갖고, 삽입시에 제거된 원소의 위치를 재사용하여 더 적은 메모리 할당/해제를 일으킵니다.
제거된 원소의 메모리 공간을 재사용하기 때문에, 정확한 삽입의 위치는 정해져있지 않습니다.
그러므로 삽입은 순서가 없지만(unordered) 컨테이너는 자체는 정렬이 가능합니다.
마지막으로, 제거되지 않은 원소들 사이 어디에서 건너뛰기가 일어날지(삭제된 원소들이 있는지) 미리 알 방법이 없기 때문에 O(1) 시간의 [] 연산은 불가능하고 
이때문에 컨테이너는 bidirectional 이지만 random-access 는 불가능합니다.

>> There are two patterns for accessing stored elements in a hive : 
the first is to iterate over the container and process each element(or skip some elements using advance / prev / next / iterator++ / --functions).
The second is to store an iterator returned by insert() (or a pointer derived from the iterator) in some other structure and access the inserted element in that way.
>
> hive 에 저장된 원소에 접근하는 것은 2가지 패턴이 있습니다:
> * 컨테이너를 순회하면서 각 요소를 처리(혹은 advance / prev / next / iterator++ / -- 함수 등을 이용해 일부 요소를 건너뜀).
> * insert() 함수가 반환한 반복자를 다른 자료구조에 저장하고 이를 이용하여 삽입된 요소에 직접 접근.