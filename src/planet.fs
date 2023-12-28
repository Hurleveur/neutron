varying vec3 TexCoords;
uniform sampler2D mytexture;

void main(void) {
    vec2 longitudeLatitude = vec2((atan(TexCoords.y, TexCoords.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(TexCoords.z) / 3.1415926 + 0.5));

    gl_FragColor = texture2D(mytexture, longitudeLatitude);
}