Phase A — Core library

 - [ ] Implement Image in image.hpp and image.cpp.
 - [ ] Create the root CMake configuration.
 - [ ] Add the simple example executable.
 - [ ] Add GoogleTest.
 - [ ] Confirm that the library builds and all tests pass.

Phase B — Image I/O

 - [ ] Integrate stb_image and stb_image_write.
 - [ ] Implement load_image() and save_image().
 - [ ] Document channel ordering and conversion behavior.
 - [ ] Add tests for loading, saving, and invalid paths.

Phase C — Benchmarking

 - [ ] Add Google Benchmark or a custom harness.
 - [ ] Implement the CPU inversion operation.
 - [ ] Benchmark inversion on several image sizes.
 - [ ] Record baseline results.
 - [ ] Prepare the benchmark interface for CUDA implementations.

Phase D — CUDA foundation

 - [ ] Enable CUDA in CMake.
 - [ ] Implement the first CUDA inversion kernel.
 - [ ] Add CPU-versus-GPU correctness tests.
 - [ ] Benchmark kernel-only and end-to-end execution.
 - [ ] Introduce persistent GPU buffers once you need to chain operations.