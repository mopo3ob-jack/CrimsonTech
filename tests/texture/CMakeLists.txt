include(crimson_project)

create_test(texture)

add_resource(shaders/shader.vert)
add_resource(shaders/shader.frag)
add_resource(textures/albedo.png)
add_resource(textures/normal.png)
add_resource(models/room.fbx)
