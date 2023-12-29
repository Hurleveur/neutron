#version 330 core
out vec4 FragColor;

struct Material {
    sampler3D diffuse;
    sampler3D specular;    
    float shininess;
}; 

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 TexCoords;
uniform sampler2D normalMap;
uniform sampler2D mytexture;
  
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform Material material;
uniform Light light;

void main() {
    vec2 longitudeLatitude = vec2((atan(TexCoords.y, TexCoords.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(TexCoords.z) / 3.1415926 + 0.5));
    // Retrieve the normal from the normal map and transform it to the range [-1, 1]
    vec3 norm = texture2D(normalMap,longitudeLatitude).rgb * 2.0 - 1.0;
    norm = normalize(norm);

    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

    vec3 result = ambient + diffuse + specular;

    FragColor = texture2D(mytexture, longitudeLatitude) + vec4(result, 1.0);
}