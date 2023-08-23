// Sezer KAYNAK

#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Shader kaynak kodları
const char* vertexShaderSource =
    "attribute vec3 vertexPosition;\n"
    "attribute vec2 vertexTexCoord;\n"
    "varying vec2 texCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(vertexPosition, 1.0);\n"
    "    texCoord = vertexTexCoord;\n"
    "}\n";

// Fragment Shader kaynak kodları
const char* fragmentShaderSource =
    "precision mediump float;\n"
    "varying vec2 texCoord;\n"
    "uniform sampler2D textureSampler;\n"
    "void main() {\n"
    "    gl_FragColor = texture2D(textureSampler, texCoord);\n"
    "}\n";

// Shader oluşturma fonksiyonu
GLuint createShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Shader derleme hatalarını kontrol etme
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Shader derleme hatası:\n%s\n", infoLog);
        return 0;
    }

    return shader;
}
// Texture ve vertex buffer nesnelerini tanımlama
GLuint vertexBuffer, texture;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // yoffset değeri mouse tekerleğinin dönüş yönüne bağlı olarak pozitif veya negatif değer alır
    // Burada üçgenlerin boyutunu yavaşça büyütüp küçültüyoruz.
    // Bu işlemi, üçgenlerin vertex verilerine müdahale ederek yapabilirsiniz.

    // Örnek olarak üçgen boyutunu yüzde 10 artırıyoruz:
    static float scale = 1.0f;
    scale += 0.1f * yoffset;

    // Bu örnekte üçgenlerin vertex verileri sabit olduğundan sadece boyutu değiştiriyoruz
    GLfloat newVertices[] = {
        -1.0f * scale, -1.0f * scale, 0.0f, 0.0f, 0.0f,
         1.0f * scale, -1.0f * scale, 0.0f, 1.0f, 0.0f,
         0.0f,  1.0f * scale, 0.0f, 0.5f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(newVertices), newVertices, GL_STATIC_DRAW);
}



int main() {

    const char* textureFile = "assets/brick.jpg";

    // GLFW'yi başlatma
    if (!glfwInit()) {
        printf("GLFW başlatılamadı.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // GLFW penceresini oluşturma
    GLFWwindow* window = glfwCreateWindow(1850, 950, "Texture Filtering", NULL, NULL);
    if (!window) {
        printf("GLFW penceresi oluşturulamadı.\n");
        glfwTerminate();
        return -1;
    }

    // OpenGL bağlantısını yapma
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);

    // Vertex Shader ve Fragment Shader oluşturma
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Shader programını oluşturma ve bağlama
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // glGetActiveAttrib ile aktif attribütlere erişme
    GLint maxAttribLength;
    glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribLength);
    char attribName[maxAttribLength];
    GLint attribSize, attribType;
    glGetActiveAttrib(shaderProgram, 0, maxAttribLength, NULL, &attribSize, &attribType, attribName);
    printf("Active Attrib: %s\nAttrib Size: %d\nAttrib Type: %d\n", attribName, attribSize, attribType);

    //glGetAttribLocation ile belirtilen attribute konumuna erişme
    GLint positionAttribLocation = glGetAttribLocation(shaderProgram, "vertexPosition");
    printf("vertexPosition konumu: %s\n", positionAttribLocation != -1 ? 
    "Attribute değişkeni shader programında bulundu." : 
    "Attribute değişkeni shader programında bulunamadı.");

    //glGetUniformLocation ile belirtilen attribute konumuna erişme
    GLint modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    printf("modelMatrix konumu: %s\n", modelMatrixLocation != -1 ? 
    "Attribute değişkeni shader programında bulundu." : 
    "Attribute değişkeni shader programında bulunamadı.");

    //glGetActiveUniform ile uniform değerlerini yazdırma
    GLint numUniforms;
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);

    GLsizei bufSize = 256;
    GLchar name[bufSize];
    GLsizei length;
    GLint size;
    GLenum type;

    for (GLuint i = 0; i < numUniforms; ++i) {
        glGetActiveUniform(shaderProgram, i, bufSize, &length, &size, &type, name);
        printf("Uniform %d: Name: %s, Size: %d, Type: 0x%x\n", i, name, size, type);
    }

    // Shader programının geçerliliğini kontrol et
    glValidateProgram(shaderProgram);

    GLint validateStatus;
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE) {
        // Shader programında geçerlilik hatası var
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader programı geçerlilik hatası:\n%s\n", infoLog);
    } else {
        printf("Shader programı sorunsuz çalışıyor.\n");
    }

    // Üçgenin köşe koordinatları ve texture koordinatları
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  1.0f, 0.0f, 0.5f, 1.0f
    };

    // Vertex Buffer oluşturma ve vertex verilerini yükleme
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex Attribute Pointerları ayarlama
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture yükleme
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    int width, height, channels;
    unsigned char* image = stbi_load(textureFile, &width, &height, &channels, STBI_rgb_alpha);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);

    // Ana döngü
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Texture binding
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);

        // İlk üçgeni çizme (GL_NEAREST ile)
        glViewport(50, 500, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // İkinci üçgeni çizme (GL_LINEAR ile)
        glViewport(500, 500, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Üçüncü üçgeni çizme (GL_NEAREST_MIPMAP_NEAREST ile)
        glViewport(950, 500, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Dördüncü üçgeni çizme (GL_LINEAR_MIPMAP_NEAREST ile)
        glViewport(1400, 500, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Beşinci üçgeni çizme (GL_NEAREST_MIPMAP_LINEAR ile)
        glViewport(50, 50, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Altıncı üçgeni çizme (GL_LINEAR_MIPMAP_LINEAR ile)
        glViewport(500, 50, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Yedinci üçgeni çizme (GL_NEAREST ile)
        glViewport(950, 50, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Sekizinci üçgeni çizme (GL_LINEAR ile)
        glViewport(1400, 50, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Çift tamponlama ve olayları işleme
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Temizleme
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    // Shader'ları silme (artık bağlı değiller)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // GLFW'yi temizleme ve çıkış
    glfwTerminate();
    return 0;
}
