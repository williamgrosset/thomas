# thomas
:train: Thomas the multi-threaded train scheduler.

### Updated Design Document
+ 5 mutexes
+ n + 2 convars (`canLoad` and `isReady` is global)
