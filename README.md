# Bowser's Raylib Utils

A collection of utils for the Raylib game library. Header only so just clone this repo into a folder and include the files you want, no other installation needed! Everything is under the `bowser_util` namespace.

(Note: **C++20 is required**)

```
├── types
│   ├── vector.h            - Math vectors (fully compatible with raylib's Vector2, Vector3, Vector4) with more functions
│   ├── bitset8.h           - Similar to std::bitset, but only occupies 8 bits instead of 64
│   ├── persistent_buffer.h - Cyclic queue of persistently mapped buffers for fast transfers from/to GPU
│   ├── spinlock.h          - Atomic spinlock, similar to std::mutex but faster for short wait times
│   └── ubo_writer.h        - Helper to write to uniform block objects (computes offsets for you)
├── graphics.h  - Graphics helpers
├── math.h      - Generic math functions, should take any numeric / float type
└── morton.h    - Morton codes (currently only for 8 bit values)
```

# Types:

## Vectors
Vectors are templated on a numeric type (ie `int` or `float`). Some methods are only available to floating point, others only to numeric template types. By default, you have `vec2`, `ivec2`, `uvec2`, `vec3`, `ivec3`, `uvec3`, `vec4`, `ivec4`, `uvec4`, which use `float`, `int`, and `unsigned int`s respectively (similar to GLSL's vectors). You can access the components with `.x, .y, .z, .w`.

Vectors can do implicit conversion, ie so if you do `vec2 v = ivec2(1)` it will cast the int vector to a float one. This goes both ways, so be careful with implicit conversions! These vectors can also implicitly cast to their raylib counterparts, so you can use these in any function that takes a raylib vector. For example, this is legal (although very so slightly inefficient due to the conversions):

```cpp
using namespace bowser_util;
vec3 a = vec3(0, 1, 2);
vec3 b = vec3(1);
vec3 c = Vector3Add(a, b); // Same as a + b
```

Of course, there is operating overloading for all the operators, ie `==` (note: exact match, use `myVec.almostEquals(otherVec)` for fuzzy match), `+=`, `+`, `-`, etc... The mod operator is also implemented for float vectors as `a % b = a - b * floor(a / b)`. Some operators such as the bitwise operators and shifts are only available for integer vectors.

**Vec2:**
Unless otherwise stated, all methods apply to both integer and floating vector variants. `T` is the base type of the vector, ie `vec3`'s is `float`.

```cpp
vec2(x, y);                                      // Construct with x, y
vec2(val);                                       // Sets x = y = val
vec2(Vector2)                                    // Construct from raylib's Vector2
vec2(vec2)                                       // Copy constructor

bool almostEquals(const vec2 &other);            // Fuzzy check if vectors are equal with raymath's float equals
float length();                                  // Get magnitude of vector
float lengthSqr();                               // Get magnitude squared of vector
float distance(vec2 &other);                     // Get distance to other vector = (other - this).length()
float distanceSqr(vec2 &other);                  // Get distance to other vector squared
T dot(const vec2 &other);                        // Dot product with other vector
float angle(const vec2 &other);                  // Angle with other vector

// These return a modified copy of the vector
vec2 reflect(const vec2 &normal);                // Return this vector reflected across a surface given its normal
vec2 rotate(float angle);                        // Get this vector rotated CW around the origin by angle (radians)
vec2 rotate(float angle, const vec2 &origin);    // Get this vector rotated CW around given origin by angle (radians)
vec2 clamp(const vec2 &v1, const vec2 &v2);      // Clamp all values between v1 and v2
vec2 clamp(const T a, const T b);                // Clamp all components between a and b
vec2 applyOp(std::function<T(T)> op);            // Return vec2(op(x), op(y))

// These are for floating point vectors only and return a modified copy
vec2 normalize();                                // Normalize to unit vector (except 0 vector -> 0 vector)
vec2 transform(const Matrix &mat);               // Transform by transformation matrix (assuming z=0, w=1)
vec2 lerp(const vec2 &target, float amount);     // Lerp amount (0 - 1) of the way to target
vec2 moveTowards(const vec2 &target, float dis); // Move fixed distance towards target (will not overshoot)
```

**Vec3:**

```cpp
vec3(x, y, z);                                   // Construct with x, y, z
vec3(val);                                       // Sets x = y = z = val
vec3(Vector3)                                    // Construct from raylib's Vector3
vec3(vec3)                                       // Copy constructor

bool almostEquals(const vec3 &other);            // Fuzzy check if vectors are equal with raymath's float equals
float length();                                  // Get magnitude of vector
float lengthSqr();                               // Get magnitude squared of vector
float distance(vec3 &other);                     // Get distance to other vector = (other - this).length()
float distanceSqr(vec3 &other);                  // Get distance to other vector squared
T dot(const vec3 &other);                        // Dot product with other vector
float angle(const vec3 &other);                  // Angle with other vector

// These return a modified copy of the vector
vec3 reflect(const vec3 &normal);                // Return this vector reflected across a surface given its normal
vec3 clamp(const vec3 &v1, const vec3 &v2);      // Clamp all values between v1 and v2
vec3 clamp(const T a, const T b);                // Clamp all components between a and b
vec3 applyOp(std::function<T(T)> op);            // Return vec3(op(x), op(y), op(z))
vec3 cross(const vec3 &other);                   // Cross product with other

vec3 rotateByQuaternion(const Quaternion &q);                 // Rotate by quaternion
vec3 rotateByAxisAngle(const vec3 &axis, float angle);        // Rotate given axis and angle
vec3 unproject(const Matrix &projection, const Matrix &view); // Unproject vector given projection and view matrix

// Refract vector given normalized surface normal, index of refraction r
// Current vector must be normalized
// Returns vec3(0) if total internal reflection would occur
vec3 refract(const vec3 &normal, const float r);

// These are for floating point vectors only and return a modified copy
vec3 normalize();                                // Normalize to unit vector (except 0 vector -> 0 vector)
vec3 transform(const Matrix &mat);               // Transform by transformation matrix (assuming w=1)
vec3 lerp(const vec3 &target, float amount);     // Lerp amount (0 - 1) of the way to target
vec3 moveTowards(const vec3 &target, float dis); // Move fixed distance towards target (will not overshoot)
```

**Vec4:**

```cpp
vec4(x, y, z, w);                                // Construct with x, y, z, w
vec4(val);                                       // Sets x = y = z = w = val
vec4(Vector4)                                    // Construct from raylib's Vector4
vec4(vec4)                                       // Copy constructor

bool almostEquals(const vec4 &other);            // Fuzzy check if vectors are equal with raymath's float equals
float length();                                  // Get magnitude of vector
float lengthSqr();                               // Get magnitude squared of vector
float distance(vec4 &other);                     // Get distance to other vector = (other - this).length()
float distanceSqr(vec4 &other);                  // Get distance to other vector squared
T dot(const vec4 &other);                        // Dot product with other vector
float angle(const vec4 &other);                  // Angle with other vector

// These return a modified copy of the vector
vec4 reflect(const vec4 &normal);                // Return this vector reflected across a surface given its normal
vec4 clamp(const vec4 &v1, const vec4 &v2);      // Clamp all values between v1 and v2
vec4 clamp(const T a, const T b);                // Clamp all components between a and b
vec4 applyOp(std::function<T(T)> op);            // Return vec4(op(x), op(y), op(z), op(w))

// These are for floating point vectors only and return a modified copy
vec4 normalize();                                // Normalize to unit vector (except 0 vector -> 0 vector)
vec4 transform(const Matrix &mat);               // Transform by transformation matrix
vec4 lerp(const vec4 &target, float amount);     // Lerp amount (0 - 1) of the way to target
vec4 moveTowards(const vec4 &target, float dis); // Move fixed distance towards target (will not overshoot)
```

## Bitset 8

A simplified version of `std::bitset` that uses 8 bits instead of 64 bits for better compacting in a struct if you ie only have an 8 bit bit flag. All bit operators are implemented for `Bitset8` with `Bitset8`. Equality can be compared with `Bitset8` or `uint8_t`, and it can implicitly be converted to `uint8_t`.

Like bitset, you can set any bit with the `[]` operator, ie:

```cpp
using namespace bowser_util;
Bitset8 bits = 0xFF;
bits[2] = false; // Unset bit 2
```

```cpp
Bitset8();                                // Default construct with data = 0
Bitset8(uint8_t data);                    // Construct with given 8 bits
std::string to_string();                  // Return string of 0s and 1s representing the data, ie "01010101"

void set();                               // Note: differs from std::bitset in that set cannot take a value for what to set
void set(std::size_t pos);                // Set given bit to 1, note: differs from std::bitset in that this only sets to true
void unset();                             // Set all bits to 0
void unset(std::size_t pos);              // Set given bit to 0

bool operator[](std::size_t pos) const;   // Return if bit at given position is set, equal to (data & (1 << pos)) != 0
reference operator[](std::size_t pos);    // Return a reference to given bit so you can set it

bool all();                               // Returns if all bits are set
bool any();                               // Returns if any bits are set
bool none();                              // Returns if no bits are set
std::size_t size();                       // Always returns 8 since it's 8 bits

myBitset8[i].flip();                      // Flips the ith bit of bitset (implemented in reference)
```

## Persistent Buffer

**MUST BE CONSTRUCTED AFTER OPENGL CONTEXT IS INITIALIZED (default constructor is fine)**

For *async* transfer from / to the GPU, you can use persistently mapped buffers. This is a class that
represents a circular buffer of OpenGL persistently mapped buffers (PMBs). This way, after you write to
one buffer, next frame you can cycle to the next one and write to that so you don't have to wait for the
first buffer to finish uploading. Usually you have three buffers (one for CPU, driver, and GPU) to triple-buffer
the upload.

Because it is async, you will have to handle the synchronization yourself. This class provides some utils for that (see examples below).

### Usage examples:

**Uploading to GPU**

```cpp
// In init after opengl context initialized
using namespace bowser_util;

unsigned int my_array[N]; // Array containing data to upload
PersistentBuffer<3> myPersistentlyMappedBuffers(GL_SHADER_STORAGE_BUFFER, sizeof(my_array), PBFlags::WRITE);

// In your game loop
myPersistentlyMappedBuffers.wait(0); // Wait for current buffer transfer to finish if it hasn't already

// Technically you do not have to copy here and can just write directly, ie
// myPersistentlyMappedBuffers.get<unsigned int>(0)[index] = value
// But writes to GPU mapped memory is about 4x slower than CPU, so if you have a lot of data
// it's much faster to batch them on the CPU then copy all at once
std::copy(
    &my_array[0],
    &my_array[N],
    &myPersistentlyMappedBuffers.get<unsigned int>(0)[0]);

// Update flag to indicate transfer has begun, don't write to buffer index 0 until wait(0) returns
myPersistentlyMappedBuffers.lock(0);
myPersistentlyMappedBuffers.advance_cycle();
```

**Downloading from GPU**

```cpp
// Init:
unsigned int my_array[N]; // Array to store downloaded data
PersistentBuffer<3> myPersistentlyMappedBuffers(GL_SHADER_STORAGE_BUFFER, sizeof(my_array), PBFlags::READ);

// In your game loop
bindBuffersForShader();
dispatchYourShaderAndStuffHere();
myPersistentlyMappedBuffers.lock(0); // Flag download has begun

// Do other CPU heavy stuff in the meantime
// Sometime later...
myPersistentlyMappedBuffers.wait(0); // Wait for download to finish if it hasn't already
std::copy(
    &myPersistentlyMappedBuffers.get<unsigned int>(1)[0],
    &myPersistentlyMappedBuffers.get<unsigned int>(1)[0] + N,
    &my_array[0]);

myPersistentlyMappedBuffers.advance_cycle();
```

**Uploading to GPU then downloading later in the same frame**

This example uses a compute shader. We write to the buffer at index 0 and read from the buffer at index 1. This
combines the above two examples:

```cpp
// Init:
unsigned int my_array[N];
PersistentBuffer<3> myPersistentlyMappedBuffers(GL_SHADER_STORAGE_BUFFER, sizeof(my_array), PBFlags::WRITE_ALT_READ);

// In your game loop
myPersistentlyMappedBuffers.wait(0); // Wait for current buffer transfer to finish if it hasn't already
// Copy data to the buffer
std::copy(&my_array[0], &my_array[N], &myPersistentlyMappedBuffers.get<unsigned int>(0)[0]);
myPersistentlyMappedBuffers.lock(0);

// Bind buffers for compute shader
rlEnableShader(myShaderProgram);
rlBindShaderBuffer(myPersistentlyMappedBuffers.getId(0), 0);
rlBindShaderBuffer(myPersistentlyMappedBuffers.getId(1), 1);
rlComputeShaderDispatch(X_WORKGROUPS, Y_WORKGROUPS, Z_WORKGROUPS);
rlDisableShader();
myPersistentlyMappedBuffers.lock(1); // Flag 1 that download is starting

// Do some other stuff in the meantime
myPersistentlyMappedBuffers.wait(1); // Wait until 1 has finished downloading
std::copy( // Copy output to my_array
    &myPersistentlyMappedBuffers.get<unsigned int>(1)[0],
    &myPersistentlyMappedBuffers.get<unsigned int>(1)[0] + N,
    &my_array[0]);

// Since we are using two buffers at once here we advance twice so next time we use buffers 2 and 3
myPersistentlyMappedBuffers.advance_cycle();
myPersistentlyMappedBuffers.advance_cycle();
```

Other notes:
- Persistently mapped buffers are not copyable, only moveable.

```cpp
// Enum for buffer usage hint, can heavily impact performance depending on your GPU
// These are just *hints* and are not enforced, but violating (ie writing to a read-only buffer)
// can incur a performance penalty. Setting a buffer to READ_AND_WRITE can also
// incur a performance penalty on some systems.

// NONE:           Unset value, do not use
// READ:           All buffers are meant to be read-only
// WRITE:          All buffers are meant to be write-only
// READ_AND_WRITE: All buffers are meant to be readable and writeable, not recommended
// WRITE_ALT_READ: Buffer 0 is writeable, buffer 1 is readable, buffer2 is writeable, etc...
// READ_ALT_WRITE: Buffer 0 is readable, buffer 1 is writeable, buffer2 is readable, etc...
enum class PBFlags { NONE, READ, WRITE, READ_AND_WRITE, WRITE_ALT_READ, READ_ALT_WRITE };

// Construct an empty persistent buffer (used as a placeholder)
// Ignores buffer count at the moment
PersistentBuffer<std::size_t bufferCount>();

// Construct a persistent buffer
// MUST BE DONE AFTER THE OPENGL CONTEXT IS INITIALIZED

// bufferCount: Number of openGL buffers to put in the cyclical buffer
// target: Buffer target, ie GL_SHADER_STORAGE_BUFFER
// size: Size in bytes of each buffer
// PBflags: Usage hints
PersistentBuffer<std::size_t bufferCount>(GLenum target, GLsizeiptr size, PBFlags rwFlag);

std::size_t getBufferCount();       // Returns bufferCount
std::size_t size();                 // Returns size in bytes of each buffer
void swap(PersistentBuffer &other); // Swap this persistent buffer with another
GLenum getTarget();                 // Get target binding

// These all take an index which is automatically offset by the cycle
// basically internally if you pass in i, it looks at dataArray[(i + cycle) % bufferCount]
void lock(std::size_t i = 0);       // Lock the ith buffer (respecting cycle count)
void wait(std::size_t i = 0);       // Wait for the ith buffer to finish processing (respecting cycle count)
void advance_cycle();               // Cycle buffers forward by 1, ie getId(0) after cycle = getId(1) before cycle
GLuint getId(std::size_t i);        // Get the ith OpenGL buffer ID, respecting cycle

 // Get pointer at index i, respecting cycle, automatically casting
 // ie myPersistentBuffer.get<unsigned int>(0) -> returns unsigned int *
template <class T = void> T * get(std::size_t i);

// Raw array bufferCount void* pointers, you can read/write from this array to
// send / receive data from the GPU (or use the methods above)
void ** ptrs = nullptr;
```

## Spinlock

A bootleg version of `std::mutex` that uses an atomic flag. Can be faster if your wait times are very short (so the OS doesn't put the thread to sleep while waiting, which can incur an overhead), but will use up your CPU since it's basically `while(true){}` while it's waiting.

Note: spinlocks cannot be copied.

Example usage (with RAII)

```cpp
using namespace bowser_util;
Spinlock lock;

void multithreadedDoSomethingWithSharedResource() {
    // Constructing a unique_spinlock will wait until it acquires the lock
    // This will automatically release the lock when out of scope
    unique_spinlock _tmp(lock);

    someSharedResource.doThing();
    // When function returns _tmp goes out of scope, and releases the lock
}
```

```cpp
// Spinlock itself
void lock();     // Spinlock, loops until the lock is acquired
bool try_lock(); // Try to acquire the lock, returns true if successful
void unlock();   // Release the lock

// RAII wrapper (also cannot be copied)
unique_spinlock(Spinlock &lock);
```

## UBOBlockWriter

**MUST BE USED AFTER OPENGL CONTEXT IS INITIALIZED**

Write to a uniform buffer object block. Automatically fetches offsets when writing to a temporary buffer
that is uploaded. Note: fetching offsets is done whenever `write_member` is called and it
takes a few to a few hundred microseconds so do not use this if you are updating the uniform data a lot
(don't do that in general).

Why does this exist? Because the `std430` memory layout can't be used by uniform blocks, and `std140`
has annoying alignment rules that can easily quadruple or more the memory requirements for uploading. So this will work
for the `shared` memory layout as well, which *should* ensure parity between the shader memory layout and the
struct layout in C.

(*I say should but this does not necessarily apply to arrays. Like `std140`, all array elements are 16 bytes regardless
of their type on some cards (*cough* AMD) even with the `shared` layout, which breaks this UBOWriter*. So **don't use arrays with this**
if you care about supporting all GPUs).

Example usage:

```cpp
// Example shader code:
// layout(shared, binding = 0) uniform NameOfTheUniformBlock {
//    int x;
//    float y;
//    bool z;
// };

// Do this after OpenGL INIT!
auto writer = UBOWriter(myShaderId, uboBufferId, "NameOfTheUniformBlock");
writer.write_member("x", 123);
writer.write_member("y", 1.23f); // Note: may not implicitly cast double->float so be careful
writer.write_member("z", 1);
writer.upload(); // Upload changes to the uniform buffer
```

Also the UBOBlockWriter can't be copied, only moved.

```cpp
// Construct:
// program          = shaderId
// UBOid            = uniform buffer object ID, ie from genBuffers
// uniformBlockName = name of the block in the shader program, see example above
UBOBlockWriter(const GLuint program, const GLuint UBOId, const char * uniformBlockName);

// Memcopy data chunk of size *size* (in bytes) from value pointer to the memberName
// There is no size check, thread carefully. Mostly used for dynamically allocated memory
template <class T> void write_member(const char * memberName, const T* value, std::size_t size);

// Write a value to memberName, assumes value is sizeof(value) bytes big
template <class T> void write_member(const char * memberName, const T &value);

void upload();                    // Upload changes to the data array to the GPU, will bind the UBO buffer
GLint size();                     // Returns total size of the UBO buffer in bytes (including padding)
void swap(UBOBlockWriter &other); // Swap with another writer
```

# Other:

## Graphics

```cpp
// Overloaded setShaderValue functions
void setShaderValue(const Shader &shaderId, int propertyLoc, Vector2 vec);
void setShaderValue(const Shader &shaderId, int propertyLoc, Vector3 vec);
void setShaderValue(const Shader &shaderId, int propertyLoc, Vector4 vec);
void setShaderValue(const Shader &shaderId, int propertyLoc, ivec2 vec);
void setShaderValue(const Shader &shaderId, int propertyLoc, ivec3 vec);
void setShaderValue(const Shader &shaderId, int propertyLoc, ivec4 vec);
void setShaderValue(const Shader &shaderId, int propertyLoc, float val);
void setShaderValue(const Shader &shaderId, int propertyLoc, int val);

// Draw render texture at location (origin) with size
// Default size is the render texture's size
void drawRenderTexture(const RenderTexture2D &tex, const Vector2 origin, const Vector2 size);
void drawRenderTexture(const RenderTexture2D &tex);
```

## Math

`T` here is any integer or floating type.

```cpp

T clamp(T val, T min, T max);                                 // Clamp val between [min, max] inclusive
T lerp(T start, T end, float amount);                         // Lerp in a range
T normalizeInRange(T value, T start, T end);                  // Remap value in range to a value in [0, 1]
T remap(T value, T start, T end, T targetStart, T targetEnd); // Remap value in [start, end] to new range [targetStart, targetEnd]
T wrap(T value, T min, T max);                                // Wrap a value in a range (similar to % but with float support)
int sign(T a);                                                // Returns 0 if a = 0, -1 if a < 0, 1 if a > 0
float rad2deg(float rad);                                     // radians -> degrees
float deg2rad(float deg);                                     // degrees -> radians

// In place modifies a transformation matrix (assuming a product of translation rotation scaling, NO SKEW)
// so it only contains the rotation matrix (if the matrix did contain skew, resultant matrix will also contain skew)
// Matrix is assumed to be used like T * pos instead of pos * T (so the translation data is in the last row)
void reduce_to_rotation(Matrix &mat);
```

## Morton.h

Morton encoding helper via lookup tables.

```cpp
namespace Morton {
    // 256 pre-transformed + shifted values for each byte for each pos
    // so final result is just X_SHIFTS[x] | Y_SHIFTS[y] | Z_SHIFTS[z]
    uint32_t X_SHIFTS[];
    uint32_t Y_SHIFTS[];
    uint32_t Z_SHIFTS[];
}

// Generate morton code for 3 8-bit unsigned coordinate values
// Morton code is xyzxyzxyz... (bits interweaved, MSB first)
// For example, if (x,y,z) = (1,2,3) the morton code would be (in binary)
// 000 ... 011 101 = 29
uint32_t morton_decode8(uint8_t x, uint8_t y, uint8_t z);
```
