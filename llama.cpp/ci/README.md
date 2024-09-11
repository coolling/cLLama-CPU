<!-- CI框架，即持续集成（Continuous Integration，简称CI），是一种软件开发实践，旨在通过自动化的方式快速且频繁地集成代码变更到共享仓库中。它通常包括自动化构建、测试和代码审查等过程，以确保代码的质量和稳定性。每次代码变更都会触发构建和测试流程，有助于及时发现和解决集成问题、减少代码冲突，并促进团队协作？ -->
# CI

In addition to [Github Actions](https://github.com/ggerganov/llama.cpp/actions) `llama.cpp` uses a custom CI framework:

https://github.com/ggml-org/ci

It monitors the `master` branch for new commits and runs the
[ci/run.sh](https://github.com/ggerganov/llama.cpp/blob/master/ci/run.sh) script on dedicated cloud instances. This allows us
to execute heavier workloads compared to just using Github Actions. Also with time, the cloud instances will be scaled
to cover various hardware architectures, including GPU and Apple Silicon instances.

Collaborators can optionally trigger the CI run by adding the `ggml-ci` keyword to their commit message.
Only the branches of this repo are monitored for this keyword.

It is a good practice, before publishing changes to execute the full CI locally on your machine:

```bash
mkdir tmp

# CPU-only build
bash ./ci/run.sh ./tmp/results ./tmp/mnt

# with CUDA support
GG_BUILD_CUDA=1 bash ./ci/run.sh ./tmp/results ./tmp/mnt

# with SYCL support
source /opt/intel/oneapi/setvars.sh
GG_BUILD_SYCL=1 bash ./ci/run.sh ./tmp/results ./tmp/mnt
```
