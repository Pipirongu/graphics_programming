#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 ShadowCoord;
in vec3 texDirection;

// Ouput data
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 positionOut;
layout(location = 2) out vec3 normalOut;

// Values that stay constant for the whole mesh.
uniform sampler2D textureSampler;
uniform sampler2D shadowMapSampler;
//G-Buffer Samplers
uniform sampler2D diffuseSampler;
uniform sampler2D positionSampler;
uniform sampler2D normalsSampler;
//Skybox Sampler
uniform samplerCube skyBoxSampler;

uniform vec3 LightPosition_worldspace;
uniform vec3 LightColor;
uniform float LightIntensity;
uniform float maxLightRadius;

uniform vec3 AmbientMaterial;
uniform vec3 SpecularMaterial;
uniform float Shininess;

//Used by Deferred Shading
uniform vec2 screenSize;
uniform mat4 viewMatrix;

//Gaussian Blur screen size
uniform vec2 ScaleU;

//Shader Mode
uniform int mode;

void main(){
	if(mode == 1){ //Shadowmap Pass 1		
		float moment1 = gl_FragCoord.z;
		float moment2 = moment1*moment1;
		
		//Bias moment2 using partial derivative to correct self-shadowing
		float dx = dFdx(moment1);
		float dy = dFdy(moment1);
		moment2 += 0.25*(dx*dx+dy*dy) ;
		
		color = vec3(moment1, moment2, 0);
	}
	else if(mode == 2){ //Shadowmap Pass 2
		// Material properties
		vec3 MaterialDiffuseColor = texture(textureSampler, UV).rgb;
		vec3 MaterialAmbientColor = AmbientMaterial * MaterialDiffuseColor;
		vec3 MaterialSpecularColor = SpecularMaterial;

		// Distance to the light
		float distance = length( LightPosition_worldspace - Position_worldspace ); //magnitude of the vector
		
		// Normal of the computed fragment, in camera space
		vec3 n = normalize( Normal_cameraspace );
		// Direction of the light (from the fragment to the light)
		vec3 l = normalize( LightDirection_cameraspace );
		
		// Cosine of the angle between the normal and the light direction, 
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendiular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp( dot( n,l ), 0,1 );
		
		// Eye vector (towards the camera)
		vec3 E = normalize(EyeDirection_cameraspace);
		// Direction in which the triangle reflects the light
		vec3 R = reflect(-l,n);
		
		// Cosine of the angle between the Eye vector and the Reflect vector,
		// clamped to 0
		//  - Looking into the reflection -> 1
		//  - Looking elsewhere -> < 1
		float cosAlpha = clamp( dot( E,R ), 0,1 );
		
		
		float pMax = 1.0;
		//Perspective division
		vec3 shadowCoordinateWdivide = ShadowCoord.xyz / ShadowCoord.w;
		vec2 UVCoords;
		//Bias to convert point to texture space
		float bias = 0.5;
		UVCoords.x = bias * shadowCoordinateWdivide.x + bias;
		UVCoords.y = bias * shadowCoordinateWdivide.y + bias;
		//Distance to Light
		float z = bias * shadowCoordinateWdivide.z + bias;
		
		//Chebyshev inequality
		vec2 moments = texture(shadowMapSampler, UVCoords).xy;
		//Compare the point with a point in shadow map
		if (moments.x < z){
			float variance = moments.y - (moments.x*moments.x);
			//Clamp the min value of variance
			variance = max(variance,0.0000005);
			float d = z - moments.x;
			//Smoothstep to correct lightbleed.
			//If pMax<=edge0 return 0, if pMax>=edge1 return 0, else return interpolated value
			pMax = smoothstep(0.2, 1.0, variance / (variance + d*d));//variance / (variance + d*d);
			//clamp the values, custom-defined min value to prevent shadows to be completely black
			pMax = clamp(pMax, 0.2, 1);
		}
		
		//Linear Attenuation, based on distance.
		//Distance is divided by the max radius of the light which must be <= scale of the light mesh
		float attenuation = clamp((1.0f - distance/40.0),0.0,1.0); 
		
		color = 
			// Ambient : simulates indirect lighting
			MaterialAmbientColor +
			// Diffuse : "color" of the object
			pMax * MaterialDiffuseColor * LightColor * LightIntensity * cosTheta * attenuation +
			// Specular : reflective highlight, like a mirror
			pMax * MaterialSpecularColor * LightColor * LightIntensity * pow(cosAlpha, Shininess) * attenuation;
	}
	else if(mode == 3){ //Render Shadowmap texture to fullscreen quad
		float depthSample = pow(texture2D(shadowMapSampler, UV).x, 10);
		color = vec3(depthSample,depthSample,depthSample);
	}
	else if(mode == 4){ //Gaussian blur
		vec3 blur = vec3(0.0);
		
		blur += texture(shadowMapSampler, UV + vec2(-3.0*ScaleU.x, -3.0*ScaleU.y)).rgb * 0.015625;
		blur += texture(shadowMapSampler, UV + vec2(-2.0*ScaleU.x, -2.0*ScaleU.y)).rgb * 0.09375;
		blur += texture(shadowMapSampler, UV + vec2(-1.0*ScaleU.x, -1.0*ScaleU.y)).rgb * 0.234375;
		blur += texture(shadowMapSampler, UV + vec2(0.0, 0.0)).rgb * 0.3125;
		blur += texture(shadowMapSampler, UV + vec2(1.0*ScaleU.x,  1.0*ScaleU.y)).rgb * 0.234375;
		blur += texture(shadowMapSampler, UV + vec2(2.0*ScaleU.x,  2.0*ScaleU.y)).rgb * 0.09375;
		blur += texture(shadowMapSampler, UV + vec2(3.0*ScaleU.x, -3.0*ScaleU.y)).rgb * 0.015625;

		color = blur;
	}
	else if(mode == 5){ //Geometry Buffer Pass
		//Output to G-Buffer attachments
		color = texture(textureSampler, UV).xyz;	
		positionOut = Position_worldspace;
		normalOut = normalize(Normal_cameraspace);				
	}
	else if(mode == 6){ //Light Pass
		//UVCoord in screenspace
		vec2 TexCoord = gl_FragCoord.xy / screenSize;
		vec3 MaterialDiffuseColor = texture(diffuseSampler, TexCoord).xyz;
		vec3 WorldPos = texture(positionSampler, TexCoord).xyz;
		vec3 Normal = texture(normalsSampler, TexCoord).xyz;

		/////
		// Material properties
		vec3 MaterialSpecularColor = SpecularMaterial;

		// Distance to the light
		float distance = length( LightPosition_worldspace - WorldPos ); //magnitude of the vector
		
		// Vector that goes from the vertex to the camera, in camera space.
		// In camera space, the camera is at the origin (0,0,0).
		vec3 vertexPosition_cameraspace = (viewMatrix * vec4(WorldPos,1)).xyz;
		vec3 cameraDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
		
		// Vector that goes from the vertex to the light, in camera space. For easier calculations later no need to get the opposite vector by then
		vec3 LightPosition_cameraspace = ( viewMatrix * vec4(LightPosition_worldspace,1)).xyz;
		vec3 LightDir_cameraspace = LightPosition_cameraspace + cameraDirection_cameraspace;
		
		// Normal of the computed fragment, in camera space
		vec3 n = normalize( Normal );
		// Direction of the light (from the fragment to the light)
		vec3 l = normalize( LightDir_cameraspace );
			
		// Cosine of the angle between the normal and the light direction, 
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendicular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp( dot( n,l ), 0,1 ); //clamp the value between 0 and 1. N dot L
		
		// Eye vector (towards the camera)
		vec3 E = normalize(cameraDirection_cameraspace);
		// Direction in which the triangle reflects the light
		vec3 R = reflect(-l,n);
		
		//vec3 H = normalize(LightDirection_cameraspace+EyeDirection_cameraspace);	
		// Cosine of the angle between the Eye vector and the Reflect vector,
		// clamped to 0
		//  - Looking into the reflection -> 1
		//  - Looking elsewhere -> < 1
		float cosAlpha = clamp( dot( E,R ), 0,1 );
		
		//Linear Attenuation, based on distance.
		//Distance is divided by the max radius of the light which must be <= scale of the light mesh
		float attenuation = clamp((1.0f - distance/maxLightRadius),0.0,1.0); 
		
		color = 
		// Diffuse : "color" of the object
		(MaterialDiffuseColor * LightColor * LightIntensity * cosTheta +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightIntensity * pow(cosAlpha, Shininess))*attenuation;
		// color = vec3(1,0,0);
		////
	}
	else if(mode == 7){ //Render diffuse color texture with ambient light to fullscreen quad
		color = AmbientMaterial*texture(diffuseSampler, UV).xyz;
	}
	else if(mode == 8){ //Render G-Buffer textures to fullscreen quad
		color = texture2D(shadowMapSampler, UV).xyz;
	}
	else if(mode == 9){ //Stencil Pass
	}
	else if(mode == 10){ //Point Light forward rendering
		// Material properties
		vec3 MaterialDiffuseColor = texture(textureSampler, UV).rgb;
		vec3 MaterialAmbientColor = AmbientMaterial * MaterialDiffuseColor;
		vec3 MaterialSpecularColor = SpecularMaterial;

		// Distance to the light
		float distance = length( LightPosition_worldspace - Position_worldspace ); //magnitude of the vector

		// Normal of the computed fragment, in camera space
		vec3 n = normalize( Normal_cameraspace );
		// Direction of the light (from the fragment to the light)
		vec3 l = normalize( LightDirection_cameraspace );
			
		// Cosine of the angle between the normal and the light direction, 
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendicular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp( dot( n,l ), 0,1 ); //clamp the value between 0 and 1. N dot L
		
		// Eye vector (towards the camera)
		vec3 E = normalize(EyeDirection_cameraspace);
		// Direction in which the triangle reflects the light
		vec3 R = reflect(-l,n);
		
		//vec3 H = normalize(LightDirection_cameraspace+EyeDirection_cameraspace);	
		// Cosine of the angle between the Eye vector and the Reflect vector,
		// clamped to 0
		//  - Looking into the reflection -> 1
		//  - Looking elsewhere -> < 1
		float cosAlpha = clamp( dot( E,R ), 0,1 );
		
		//Linear Attenuation, based on distance.
		//Distance is divided by the max radius of the light which must be <= scale of the light mesh
		float attenuation = clamp((1.0f - distance/360.0),0.0,1.0); 
		
		color = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		(MaterialDiffuseColor * LightColor * LightIntensity * cosTheta +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightIntensity * pow(cosAlpha, Shininess))*attenuation;
	}
	else if(mode == 11){ //Skybox
		color = texture(skyBoxSampler, texDirection).rgb;
	}
	else if(mode == 12){ //Wireframe
		color = vec3(0.0, 0.3, 0.0);
	}
	else if(mode == 13){ //No Light
		 color = AmbientMaterial * texture(textureSampler, UV).rgb;
	}
}