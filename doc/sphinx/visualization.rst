.. _Online-visualization:

Online-visualization
====================

|es| offers a direct rendering engine based on *pyopengl*.
It supports several features like shape-based constraints,
particle properties, the cell system, lattice-Boltzmann and more.
It can be adjusted with a large number of parameters to set colors,
materials, camera and interactive features like assigning callbacks
to user input. It requires the Python module *PyOpenGL*.
It is not meant to produce high quality renderings, but rather to
debug the simulation setup and equilibration process.

.. _General usage:

General usage
-------------

The recommended usage is to instantiate the visualizer and pass it the
:class:`espressomd.System() <espressomd.system.System>` object. Then write
your integration loop in a separate function, which is started in a
non-blocking thread. Whenever needed, call ``update()`` to synchronize
the renderer with your system. Finally start the blocking visualization
window with ``start()``. See the following minimal code example::

    import espressomd
    import espressomd.visualization
    import threading

    system = espressomd.System(box_l=[10, 10, 10])
    system.cell_system.skin = 0.4
    system.time_step = 0.00001

    system.part.add(pos=[1, 1, 1], v=[1, 0, 0])
    system.part.add(pos=[9, 9, 9], v=[0, 1, 0])

    visualizer = espressomd.visualization.openGLLive(system)

    def main_thread():
        while True:
            system.integrator.run(1)
            visualizer.update()

    t = threading.Thread(target=main_thread)
    t.daemon = True
    t.start()
    visualizer.start()

.. _Setting up the visualizer:

Setting up the visualizer
-------------------------

:class:`espressomd.visualization.openGLLive()`

The required parameter  ``system`` is the :class:`~espressomd.system.System` object.
The optional keywords in ``**kwargs`` are used to adjust the appearance of the visualization.
These parameters have suitable default values for most simulations.

:meth:`espressomd.visualization.openGLLive.update()`

``update()`` synchronizes system and visualizer, handles keyboard events for
openGLLive.

:meth:`espressomd.visualization.openGLLive.start()`

``start()`` starts the blocking visualizer window.
Should be called after a separate thread containing ``update()`` has been started.

:meth:`espressomd.visualization.openGLLive.register_callback()`

Registers the method ``callback()``, which is called every ``interval`` milliseconds. Useful for
live plotting (see sample script :file:`/samples/visualization_ljliquid.py`).

.. note::

  The visualization of some constraints is either improved by (:class:`espressomd.shapes.SimplePore`)
  or even relies on (:class:`espressomd.shapes.HollowConicalFrustum`) the presence of an installed
  `OpenGL Extrusion library` on your system. Typically, the library will be available through the
  default package manager of your operating system. On Ubuntu the required package is called ``libgle3-dev``,
  on Fedora ``libgle`` -- just to name two examples.


.. _Running the visualizer:

Running the visualizer
~~~~~~~~~~~~~~~~~~~~~~

:meth:`espressomd.visualization.openGLLive.run()`

To visually debug your simulation, ``run(n)`` can be used to conveniently start
an integration loop with ``n`` integration steps in a separate thread once the
visualizer is initialized::

    import espressomd
    import espressomd.visualization

    system = espressomd.System(box_l=[10, 10, 10])
    system.cell_system.skin = 0.4
    system.time_step = 0.0001

    system.part.add(pos=[1, 1, 1], v=[1, 0, 0])
    system.part.add(pos=[9, 9, 9], v=[0, 1, 0])

    visualizer = espressomd.visualization.openGLLive(system, background_color=[1, 1, 1])
    visualizer.run(1)


.. _Screenshots:

Screenshots
~~~~~~~~~~~

| :meth:`espressomd.visualization.openGLLive.screenshot()`

The OpenGL visualizer can also be used for offline rendering.
After creating the visualizer object, call ``screenshot(path)``
to save an image of your simulation to ``path``. Internally, the image is saved
with ``matplotlib.pyplot.imsave``, so the file format is specified by the
extension of the filename.  The image size is determined by the keyword
argument ``window_size`` of the visualizer. This method can be used to create
screenshots without blocking the simulation script::

    import espressomd
    import espressomd.visualization

    system = espressomd.System(box_l=[10, 10, 10])
    system.cell_system.skin = 1.0
    system.time_step = 0.1

    for i in range(1000):
        system.part.add(pos=[5, 5, 5])

    system.thermostat.set_langevin(kT=1, gamma=1, seed=42)

    visualizer = espressomd.visualization.openGLLive(system, window_size=[500, 500])

    for i in range(100):
        system.integrator.run(1)
        visualizer.screenshot(f'screenshot_{i:0>5}.png')

    # You may consider creating a video with ffmpeg:
    # ffmpeg -f image2 -framerate 30 -i 'screenshot_%05d.png' output.mp4

It is also possible to create a snapshot during online visualization.
Simply press the *enter* key to create a snapshot of the current window,
which saves it to :file:`<scriptname>_n.png` (with incrementing ``n``).

.. _Colors and Materials:

Colors and Materials
~~~~~~~~~~~~~~~~~~~~

Colors for particles, bonds and constraints are specified by RGB arrays.
Materials by an array for the ambient, diffuse, specular and shininess and opacity (ADSSO)
components. To distinguish particle groups, arrays of RGBA or ADSSO entries are
used, which are indexed circularly by the numerical particle type::

    # Particle type 0 is red, type 1 is blue (type 2 is red etc)..
    visualizer = espressomd.visualization.openGLLive(system,
                                                     particle_coloring='type',
                                                     particle_type_colors=[[1, 0, 0], [0, 0, 1]])

``particle_type_materials`` lists the materials by type::

    # Particle type 0 is gold, type 1 is blue (type 2 is gold again etc).
    visualizer = espressomd.visualization.openGLLive(system,
                                                     particle_coloring='type',
                                                     particle_type_colors=[[1, 1, 1], [0, 0, 1]],
                                                     particle_type_materials=["steel", "bright"])

Materials are stored in :attr:`espressomd.visualization.openGLLive.materials`.

.. _Visualize vectorial properties:

Visualize vectorial properties
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Most vectorial particle properties can be visualized by 3D-arrows on the
particles:

* ``ext_force``: An external force. Activate with the keyword ``ext_force_arrows = True``.
* ``f``: The force acting on the particle. Activate with the keyword ``force_arrows = True``.
* ``v``: The velocity of the particle. Activate with the keyword ``velocity_arrows = True``.
* ``director``: A vector associated with the orientation of the particle. Activate with the keyword ``director_arrows = True``.

Arrow colors, scales and radii can be adjusted. Again, the lists specifying
these quantities are indexed circularly by the numerical particle type. The
following code snippet demonstrates the visualization of the director property
and individual settings for two particle types (requires the ``ROTATION``
feature)::

    import numpy as np
    import espressomd
    from espressomd.visualization import openGLLive, KeyboardButtonEvent, KeyboardFireEvent

    box_l = 10
    system = espressomd.System(box_l=[box_l, box_l, box_l])
    system.cell_system.skin = 0.4

    system.time_step = 0.00001

    visualizer = openGLLive(system,
                            director_arrows=True,
                            director_arrows_type_scale=[1.5, 1.0],
                            director_arrows_type_radii=[0.1, 0.4],
                            director_arrows_type_colors=[[1.0, 0, 0], [0, 1.0, 0]])

    for i in range(10):
        system.part.add(pos=np.random.random(3) * box_l,
                        rotation=[1, 1, 1],
                        ext_torque=[5, 0, 0],
                        v=[10, 0, 0],
                        type=0)
        system.part.add(pos=np.random.random(3) * box_l,
                        rotation=[1, 1, 1],
                        ext_torque=[0, 5, 0],
                        v=[-10, 0, 0],
                        type=1)

    visualizer.run(1)




.. _Controls:

Controls
~~~~~~~~

The camera can be controlled via mouse and keyboard:

* hold left button: rotate the system
* hold right button: translate the system
* hold middle button: zoom / roll
* mouse wheel / key pair TG: zoom
* WASD-Keyboard control (WS: move forwards/backwards, AD: move sidewards)
* Key pairs QE, RF, ZC: rotate the system
* Double click on a particle: Show particle information
* Double click in empty space: Toggle system information
* Left/Right arrows: Cycle through particles
* Space: If started with ``run(n)``, this pauses the simulation
* Enter: Creates a snapshot of the current window and saves it to :file:`<scriptname>_n.png` (with incrementing ``n``)

Additional input functionality for mouse and keyboard is possible by assigning
callbacks to specified keyboard or mouse buttons. This may be useful for
realtime adjustment of system parameters (temperature, interactions, particle
properties, etc.) or for demonstration purposes. The callbacks can be triggered
by a timer or keyboard input::

    def foo():
        print("foo")

    # Registers timed calls of foo()
    visualizer.register_callback(foo, interval=500)

    # Callbacks to control temperature
    temperature = 1.0
    system.thermostat.set_langevin(kT=temperature, seed=42, gamma=1.0)
    def increaseTemp():
        global temperature
        temperature += 0.5
        system.thermostat.set_langevin(kT=temperature, gamma=1.0)
        print(f"T = {system.thermostat.get_state()[0]['kT']:.1f}")

    def decreaseTemp():
        global temperature
        temperature -= 0.5
        if temperature > 0:
            system.thermostat.set_langevin(kT=temperature, gamma=1.0)
            print(f"T = {system.thermostat.get_state()[0]['kT']:.1f}")
        else:
            temperature = 0
            system.thermostat.turn_off()
            print("T = 0")

    # Registers input-based calls with keys Y and H
    visualizer.keyboard_manager.register_button(KeyboardButtonEvent('y', KeyboardFireEvent.Hold, increaseTemp))
    visualizer.keyboard_manager.register_button(KeyboardButtonEvent('h', KeyboardFireEvent.Hold, decreaseTemp))

    visualizer.run(1)

Further examples can be found in :file:`/samples/billiard.py` or :file:`/samples/visualization_interactive.py`.

.. _Dragging particles:

Dragging particles
~~~~~~~~~~~~~~~~~~

With the keyword ``drag_enabled`` set to ``True``, the mouse can be used to
exert a force on particles in drag direction (scaled by ``drag_force`` and the
distance of particle and mouse cursor).

.. _Visualization example scripts:

Visualization example scripts
-----------------------------

Various :ref:`Sample Scripts` can be found in :file:`/samples/visualization_*.py`
or in the :ref:`Tutorials` "Visualization" and "Charged Systems".
