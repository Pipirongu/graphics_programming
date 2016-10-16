#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
//Point in light space to compare with point in shadow map
out vec4 ShadowCoord;
//UV for skybox
out vec3 texDirection;

// Values that stay constant for the whole mesh.
uniform mat4 mvp;
uniform mat4 depthMVP;
uniform mat4 view;
uniform mat4 model;
uniform mat4 modelView;
uniform vec3 LightPosition_worldspace;

uniform int mode;

void main(){
	if(mode == 1){ //Shadowmap Pass 1
		gl_Position = depthMVP * vec4(vertexPosition_modelspace,1);	
	}
	else if(mode == 2){ //Shadowmap Pass 2
		// Output position of the vertex, in clip space : MVP * position
		gl_Position =  mvp * vec4(vertexPosition_modelspace,1);
		ShadowCoord = depthMVP * vec4(vertexPosition_modelspace,1);
		
		// Position of the vertex, in worldspace : M * position
		Position_worldspace = (model * vec4(vertexPosition_modelspace,1)).xyz;
		
		// Vector that goes from the vertex to the camera, in camera space.
		// In camera space, the camera is at the origin (0,0,0).
		vec3 vertexPosition_cameraspace = (modelView * vec4(vertexPosition_modelspace,1)).xyz;
		EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
		
		// Vector that goes from the vertex to the light, in camera space. For easier calculations later no need to get the opposite vector by then
		vec3 LightPosition_cameraspace = ( view * vec4(LightPosition_worldspace,1)).xyz;
		LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
		
		// Normal of the the vertex, in camera space
		Normal_cameraspace = (modelView * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
		
		// UV of the vertex. No special space for this one.
		UV = vertexUV;
	}
	else if(mode == 3){ //Render Shadowmap texture to fullscreen quad
		gl_Position =  vec4(vertexPosition_modelspace,1);
		UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;
	}
	else if(mode == 4){ //Gaussian blur		
		gl_Position =  vec4(vertexPosition_modelspace,1);
		UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;
	}
	else if(mode == 5){ //Geometry Buffer Pass
		gl_Position = mvp * vec4(vertexPosition_modelspace, 1.0);
		
		//Output to G-Buffer attachments
		UV = vertexUV;
		Position_worldspace = (model * vec4(vertexPosition_modelspace,1)).xyz;
		Normal_cameraspace = (modelView * vec4(vertexNormal_modelspace,0)).xyz;
	}
	else if(mode == 6){ //Light Pass
		gl_Position = mvp * vec4(vertexPosition_modelspace, 1.0);
	}
	else if(mode == 7){ //Render diffuse color texture with ambient light to fullscreen quad
		gl_Position =  vec4(vertexPosition_modelspace,1);
		UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;
	}
	else if(mode == 8){ //Render G-Buffer textures to fullscreen quad
		gl_Position =  vec4(vertexPosition_modelspace,1);
		UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;
	}
	else if(mode == 9){ //Stencil Pass
		gl_Position = mvp * vec4(vertexPosition_modelspace,1);	
	}
	else if(mode == 10){ //Point Light forward rendering
		//Output position of vertex in clip space
		gl_Position = mvp * vec4(vertexPosition_modelspace, 1);

		// Position of the vertex, in worldspace : model * position
		Position_worldspace = (model * vec4(vertexPosition_modelspace,1)).xyz;
		
		// Vector that goes from the vertex to the camera, in camera space.
		// In camera space, the camera is at the origin (0,0,0).
		vec3 vertexPosition_cameraspace = ( modelView * vec4(vertexPosition_modelspace,1)).xyz;
		EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

		// Vector that goes from the vertex to the light, in camera space. For easier calculations later no need to get the opposite vector by then
		vec3 LightPosition_cameraspace = ( view * vec4(LightPosition_worldspace,1)).xyz;
		LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

		// Normal of the the vertex, in camera space
		Normal_cameraspace = ( modelView * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model non-uniformly! If so then use its inverse transpose.

		//UVs are sent to the fragment shader
		UV = vertexUV;
	}
	else if(mode == 11){ //Skybox
	    vec4 mvpPos = mvp * vec4(vertexPosition_modelspace,1);   
		//Supply w as z to make sure z is always 1 so that the skybox will always fail the depth test
		//The skybox will then only be rendered wherer there are no models. As long as the skybox is rendered last
		gl_Position = mvpPos.xyww;  
		
		//Cube sampler will decide which face to sample from based on the highest value component
		texDirection = vertexPosition_modelspace;
	}
	else if(mode == 12){ //Wireframe
		gl_Position = mvp * vec4(vertexPosition_modelspace,1);
	}
	else if(mode == 13){ //No Light
	    gl_Position = mvp * vec4(vertexPosition_modelspace,1);
		//UVs are sent to the fragment shader
		UV = vertexUV;
	}
}