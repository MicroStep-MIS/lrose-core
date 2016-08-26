# lrose-core

![SPOL](./docs/images/spol_dynamo.jpg)

## **L**:rose: - The Lidar Radar Open Software Environment

LROSE is a co-operative project between:

  * [Dept. of Atmospheric Science at Colorado State University (CSU)](http://www.atmos.colostate.edu/) and the
  * [The Earth Observing Lab at the National Center for Atmospheric Research (NCAR)](https://www.eol.ucar.edu/content/lidar-radar-open-software-environment).

LROSE is funded by the [National Science Foundation](https://www.nsf.gov).

### LROSE source in GitHub

This distribution contains the core software for LROSE.

To download LROSE, run:

```
git clone https://github.com/NCAR/lrose-core
```

### Organization of the LROSE repository

| Location      | Description   |
| ------------- |:-------------:|
| docs          | README files |
| release_notes | release notes for the various packages in LROSE |
| build         | build scripts, release scripts and associated files |
| codebase/apps | application source code |
| codebase/libs | support library source code |
| codebase/make_bin | release and build support scripts |
| codebase/make_include | incude files for NCAR-style make system |

### Building LROSE

You can build LROSE using either of the following:

  * NCAR development environment build
  * automake configure-based build
  * download pre-configured source distribution

If you are performing development, it is best to use the NCAR development environment.

If you just wish to build and use the software, the automake/configure build is preferable.

See [README_BUILD.md](./docs/README_BUILD.md) for details.






