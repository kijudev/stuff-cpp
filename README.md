# Stuff CPP

A proof-of-concept sparse set ECS. Stuff = Things and Parts. Things are well, just things, unique and monotonic-like ids that represent objects in the world. Parts are the actuall data, they can be attached to things. We can query for parts, for example we can tell the system: Give me all the things (with the requested data) that have `Transform` and `Sprite` parts.

