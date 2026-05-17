Noxim + Circulant
=========================

This repository contains a modified version of **Noxim**, a Network-on-Chip simulator developed at the University of Catania.

The modification adds support for a circulant topology of the form `C(N; 1, s)` and a routing algorithm for this topology. The work was carried out as part of a master's thesis on the research and modification of high-level models of Networks-on-Chip.

## Main modifications

- Added support for the `CIRCULANT` topology.
- Added the `circulant_step` configuration parameter.
- Implemented construction of circulant connections for the topology `C(N; 1, s)`.
- Added the `Routing_CIRCULANT` routing algorithm.
- Added example configuration files for mesh and circulant experiments.

## Repository structure

- `src/` — source code of the modified simulator.
- `src/routingAlgorithms/` — routing algorithms, including `Routing_CIRCULANT`.
- `config_examples/` — configuration examples.
- `config_examples/experiments/` — configuration files used for mesh and circulant experiments.
- `bin/` — build Makefile and power configuration.

## Example run

From the `bin` directory:

```bash
make
./noxim -config ../config_examples/experiments/experiment_configMesh.yaml
./noxim -config ../config_examples/experiments/experiment_configCirculant.yaml
```

## Original Noxim

The original Noxim simulator was developed by the University of Catania.

Original project: https://github.com/davidepatti/noxim

If you use Noxim in your research, please cite the original authors according to the recommendations provided in the original Noxim documentation.

## License

The original Noxim project is distributed under the GPL license terms. This repository preserves the original project documentation and license files in the `doc/` directory.