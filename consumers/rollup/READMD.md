# rollup


This consumer emits assets as soon as the criteria defined in activator_criteria and emitter_criteria is met.

Use cases -
1) emit asset as an account receives a group of assets.
2) create a dag like structure for asset emission using combination of activators and emitters.

Cyclic rollups (ie emitting same assets multiple times) can be turned on using cyclic emitters and activator boolean field.
