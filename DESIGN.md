# M2 Technical & Design Understanding

This file is an assessed technical-understanding artifact, not ordinary project documentation.
Answer all four questions using your own submitted implementation. Concise answers are acceptable when they are technically correct and specific.

Generic descriptions of C++ concepts or restatements of the assignment that do not identify and explain corresponding parts of your code will receive limited credit.

## 1. Polymorphism and dynamic dispatch - 1.5 points

Question:
Identify one place in your M2 implementation where runtime polymorphism occurs. Name the relevant base interface, derived implementation, and `ProcessingCore` function involved. Trace the call from `ProcessingCore` to the selected strategy implementation and explain why the derived implementation is invoked.

Answer:
Runtime polymorphism happens in `ProcessingCore::rebuild()`. 
The base interface is `ChunkingStrategy` and the default derived implementation is `Chunker`.

`ProcessingCore::Impl` stores the strategy as:
`std::unique_ptr<ChunkingStrategy>`

Inside `rebuild()` the call is:
`impl_->chunking->chunk(doc, order)`

Because `chunk()` is declared `virtual` in `ChunkingStrategy`, the code checks the actual object stored in the pointer at runtime. With the default configuration, that object is a `Chunker`, so `Chunker::chunk()` is called.

In my student test, I injected a `CustomChunkingStrategy` instead, so the same call invokes that implementation. 

Question:
Then explain what would change if the relevant operation were not declared `virtual`.

Answer:
If `chunk()` were not virtual, the call would not use dynamic dispatch, so the derived strategy implementation would not be selected at runtime.


## 2. Ownership and lifetime - 1.5 points

Question:
Identify where one of the strategy objects is created, where ownership is transferred, and which object ultimately owns it. Explain how `std::unique_ptr` represents that ownership relationship and when the strategy object is destroyed.

Answer:
A strategy object is created in the default `ProcessingCore` constructor with:
`std::make_unique<Chunker>(...)`

That `std::unique_ptr` is passed into the constructor that accepts the three strategy interfaces. Ownership is then transferred into `ProcessingCore::Impl` with `std::move()`.

`Impl` ultimately owns the chunking strategy through:
`std::unique_ptr<ChunkingStrategy> chunking;`

`std::unique_ptr` represents exclusive ownership, which means only one owner controls that object at a time.

When `ProcessingCore` and its `Impl` object are destroyed, the `unique_ptr` automatically destroys the strategy object.

Question:
Also explain why `ProcessingCore` is move-only and why the strategy base classes require virtual destructors.

Answer:
`ProcessingCore` is move-only because it owns `std::unique_ptr` members indirectly through `Impl`, and `unique_ptr` cannot be copied.

Moving is allowed because ownership can be transferred safely from one object to another. The strategy base classes require virtual destructors because derived objects such as `Chunker` can be destroyed through base-class pointers like `ChunkingStrategy`. The virtual destructor makes sure the correct derived object is fully destroyed.


## 3. Architecture, extensibility, and M1 compatibility - 1.5 points

Question:
Explain one specific architectural decision in your M2 implementation that makes the processing system extensible while preserving M1 behavior.

Answer:
A specific architectural decision in my M2 implementation was replacing direct concrete processing objects with strategy interfaces.

Now, `ProcessingCore` stores:
- `ChunkingStrategy`
- `RetrievalStrategy`
- `ContextStrategy`

through `std::unique_ptr`.

Question:
Identify the classes or interfaces involved and explain both:
- how the default configuration preserves M1 behavior; and
- how a different implementation can be substituted without changing the normal `ProcessingCore` API.

Answer:
The default constructor still creates the original M1 implementation:
- `Chunker`
- `RetrievalEngine`
- `ContextBuilder`

Because those same concrete classes are still used by default, the normal M1 behavior is preserved. At the same time, a different implementation can be passed into the `ProcessingCore` constructor without changing the normal public API.

The code can still call:
- `rebuild()`
- `search()`
- `build_context()`

without needing to know which strategy implementation is being used.

Question:
Include one plausible design alternative and explain why the M2 design is preferable for this milestone. The alternative does not need to be something you actually implemented.

Answer:
One plausible design alternative would be to keep `Chunker`, `RetrievalEngine`, and `ContextBuilder` directly inside `ProcessingCore`. This would be simpler, but it would hold `ProcessingCore` to those exact implementations. The M2 strategy design is preferable because it preserves the original default behavior while also allowing runtime substitution.


## 4. Testing and defect reasoning - 1.5 points

Question:
Select one meaningful test from your `tests/student_tests.cpp`.

Explain:
- what M2 requirement the test validates;
- what specific implementation defect the test could detect; and
- why your test provides useful evidence beyond simply rerunning the supplied public tests.

Answer:
One meaningful test I added is:
`test_custom_chunking_strategy_is_used()`

The test validates the M2 requirement that a custom strategy can be injected and used at runtime.

My `CustomChunkingStrategy` returns a chunk with:
- An ID ending in `-custom`.
- The text `custom strategy output`.

In the code, I inject that strategy into `ProcessingCore` and then call `rebuild()`. The test checks that the resulting chunk contains the custom ID and custom text.

The test gives useful evidence beyond only rerunning the public tests because it uses a strategy I created with clear and observable behavior.

Question:
If your test uses a custom strategy, explain how its observable behavior demonstrates that `ProcessingCore` is actually using runtime substitution.

Answer:
If the custom output appears, it shows that `ProcessingCore` is actually using runtime substitution through the `ChunkingStrategy` interface.