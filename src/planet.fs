varying vec4 texCoords;
uniform sampler2D mytexture;

void main(void) {
    vec2 longitudeLatitude = vec2((atan(texCoords.y, texCoords.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(texCoords.z) / 3.1415926 + 0.5));

    gl_FragColor = texture2D(mytexture, longitudeLatitude);
}