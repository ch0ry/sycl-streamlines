# Proyecto de grado

## Título del proyecto: Estudio experimental sobre el impacto del uso de SYCL en el cálculo de Streamlines, Streaklines y Pathlines

## Objetivo general

Comparar el desempeño computacional de una técnica de visualización de campos de flujo (Streamlines, Streaklines y Pathlines), que involucra el método de Runge-Kutta implementado en lenguaje C/C++, mediante métodos tradicionales de cómputo y programación heterogénea con SYCL, evaluando métricas como el tiempo de ejecución, uso de memoria y escalabilidad.

## Justificación

La visualización de campos de flujo, mediante técnicas como Streamlines, Streaklines y Pathlines, es fundamental en diversas áreas de la ciencia e ingeniería, incluyendo la dinámica de fluidos computacional (CFD), la meteorología, y el modelado de procesos físicos (Camp et al., 2011). Estos métodos permiten representar de forma intuitiva el comportamiento de partículas en movimiento dentro de un campo vectorial, lo cual facilita el análisis y la interpretación de fenómenos complejos.

Sin embargo, la simulación precisa y eficiente de estas trayectorias implica un alto costo computacional, especialmente cuando se trabaja con volúmenes de datos grandes o se requieren resoluciones espaciales y temporales finas (Pugmire et al., 2009). En este contexto, la programación tradicional en C/C++, aunque eficiente, puede resultar insuficiente para explotar de manera óptima las capacidades de cómputo de los sistemas modernos, en especial aquellos que cuentan con aceleradores como GPUs.

A pesar de sus beneficios teóricos, existe una brecha de conocimiento práctico respecto al impacto real del uso de SYCL en problemas computacionalmente intensivos como el cálculo de líneas de flujo. No se cuenta con suficiente evidencia comparativa que cuantifique su desempeño frente a enfoques tradicionales en términos de tiempo de ejecución, consumo de memoria y escalabilidad, con respecto a esta técnica de visualización en particular. Para cuantificar el desempeño se realizan estudios experimentales usando métricas como tiempo de ejecución, uso de memoria y escalabilidad (Camp et al., 2013), relacionadas con características del problema como: tamaño del dataset, tamaño del conjunto de semillas, distribución del conjunto de semillas y complejidad del campo vectorial (Camp et al., 2011).

Camp, D., Garth, C., Childs, H., Pugmire, D., & Joy, K. (2011). Streamline Integration Using MPI-Hybrid Parallelism on a Large Multicore Architecture. IEEE Transactions on Visualization and Computer Graphics, 17(11), 1702–1713. <https://doi.org/10.1109/TVCG.2010.259>

Camp, D., Krishnan, H., Pugmire, D., Garth, C., Johnson, I., Bethel, E. W., Joy, K. I., & Childs, H. (2013). GPU Acceleration of Particle Advection Workloads in a Parallel, Distributed Memory Setting. In K. Moreland & F. Marton (Eds), Eurographics Symposium on Parallel Graphics and Visualization. The Eurographics Association. <https://doi.org/10.2312/EGPGV/EGPGV13/001-008>

Keryell, R. (2019). SYCL A Single-Source C++ Standard for Heterogeneous Computing. <https://www.khronos.org/assets/uploads/developers/presentations/SC19-H2RC-keynote-SYCL_Nov19.pdf>

Pugmire, D., Childs, H., Garth, C., Ahern, S., & Weber, G. H. (2009). Scalable computation of streamlines on very large datasets. Proceedings of the Conference on High Performance Computing Networking, Storage and Analysis, 1–12. <https://doi.org/10.1145/1654059.1654076>

## Objetivos específicos

- Formular una versión optimizada del algoritmo RK4 para ejecución en arquitecturas heterogéneas utilizando SYCL.
- Comparar el comportamiento de las implementaciones bajo diferentes características del problema.
- Examinar los resultados obtenidos para determinar ventajas, limitaciones y condiciones óptimas de uso de SYCL frente a soluciones de solo CPU o de GPU de uso común en la industria como CUDA.

# Observaciones

7- Aunque los OE incluidos no hacen parte de esta evaluación, se hace la observación que los mismos están formulados como actividades y no como Objetivos. NOTA: Se ha de tener presente esta observación en la formulación del Plan

# Referencias

- Hernandez Ariza, M. L., Dreher, M., Barrios-Hernandez, C. J., & Raffin, B. (2015, agosto). Asynchronous In Situ Processing with Gromacs: Taking Advantage of GPUs. Latin America High Performance Computing Conference. <https://hal.inria.fr/hal-01180364>

- Jain, S., Tripathi, N., & Narayanan, P. J. (2014). Interactive Simulation of Generalised Newtonian Fluids Using GPUs. Proceedings of the 2014 Indian Conference on Computer Vision Graphics and Image Processing, 79:1-79:8. <https://doi.org/10.1145/2683483.2683562>

- Baracca, M. C., Clai, G., & Ornelli, P. (2001). In-situ Bioremediation: Advantages of Parallel Computing and Graphical Investigating Techniques. En V. N. Alexandrov, J. J. Dongarra, B. A. Juliano, R. S. Renner, & C. J. K. Tan (Eds.), Computational Science—ICCS 2001 (pp. 99-107). Springer Berlin Heidelberg.

- Michéa, D., Pouderoux, J., Dupros, F., & Aochi, H. (2013). Steering and In-situ Visualization for Simulation of Seismic Wave Propagation on Graphics Cards. En P. Manninen & P. Öster (Eds.), Applied Parallel and Scientific Computing (pp. 107-114). Springer Berlin Heidelberg.

- Carlotto, T., Silva, R. V. da, & Grzybowski, J. M. V. (2019). GPGPU-accelerated environmental modelling based on the 2D advection-reaction-diffusion equation. Environmental Modelling & Software. <https://doi.org/10.1016/j.envsoft.2019.02.001>

- Camp, D., Garth, C., Childs, H., Pugmire, D., & Joy, K. (2011). Streamline Integration Using MPI-Hybrid Parallelism on a Large Multicore Architecture. IEEE Transactions on Visualization and Computer Graphics, 17(11), 1702–1713. <https://doi.org/10.1109/TVCG.2010.259>

- Pugmire, D., Childs, H., Garth, C., Ahern, S., & Weber, G. H. (2009). Scalable computation of streamlines on very large datasets. Proceedings of the Conference on High Performance Computing Networking, Storage and Analysis, 1–12. <https://doi.org/10.1145/1654059.1654076>
