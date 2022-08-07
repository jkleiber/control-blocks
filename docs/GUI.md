# GUI Design

The GUI will be made in C++ using [Dear ImGui](https://github.com/ocornut/imgui).


Python was originally considered for writing the gui, but it is desired to make this an executable application 
rather than a script. Python GUI frameworks considered:
- [Dear PyGui](https://github.com/hoffstadt/DearPyGui/)
- Kivy: This looks like it might be easy to use, however it seems mostly optimized for mobile
- PyQt: I've used this before, but I didn't particularly like it. It also has a lot of licensing requirements, 
and I want commercial users to be able to use this project.
- PySimpleGUI: Might be too simple? Didn't investigate very thoroughly

## GUI Requirements
1. The GUI shall provide a means to save diagrams and load saved diagrams.
2. The GUI shall provide a canvas for editing the diagram
    a. The canvas shall be zoomable and scrollable
    b. The canvas shall allow subdiagrams, each with their own canvas
3. The GUI shall enable the manipulation of blocks and wires
    a. Blocks shall have input and output nodes
    b. Blocks shall be connected to other blocks using wires
    c. Blocks and wires shall be editable after placement.
4. The GUI shall have a toolbar for adding blocks to the canvas