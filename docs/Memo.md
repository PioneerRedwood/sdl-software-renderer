# Memo
- Rasterization: 3차원 공간에 존재하는 정점을 어떤 과정을 거쳐서 2차원 화면에 그리는 것
- OpenGL 기준 좌표계를 사용하여 SDL만을 갖고 렌더링하는 것이 목표
- 해당 메모에서는 그 과정에 찾아본 것을 정리

## OpenGL Rendering Pipeline
해당 사이트에 있는 그림을 참고하는 것을 추천

- OpenGL 파이프라인은 일종의 순서가 있는 처리 스테이지로 구성 
- 정점 기반 데이터와 픽셀 기반 데이터, 두 종류는 이 파이프라인에서 처리되고 조합하여 프레임 버퍼에 쓰여짐
- OpenGL은 처리된 데이터를 애플리케이션 쪽으로도 반대로 가져올 수 있음

### Display List
- 후에 실행하기 위해 저장된(컴파일된) OpenGL 명령 그룹
- 모든 데이터, 기하 정보(정점) 그리고 픽셀 데이터는 여기에 저장될 수 있음
- 명령과 데이터 모두 캐시될 수 있기 때문에 성능 향상될 수 있음
- 가능하면 여기에 한번 수행되고 수정하지 않는 일종의 명령 그룹을 저장하는 것을 권장 
- 자세한 것은 문서 확인 [OpenGL Display List](http://www.songho.ca/opengl/gl_displaylist.html)

### Vertex Operation
- 각 정점과 일반적인 좌표가 `GL_MODELVIEW` 매트릭스 (모델 좌표계 -> 카메라 좌표계로 변환)에 의해 전송됨
- 만약 라이팅이 활성화되어 있다면, 각 정점의 라이팅 계산은 전송된 정점과 일반 좌표을 사용함

### Primitives Assembly
- Vertex Operation이 종료된 후 프리미티브들 (점, 선 그리고 폴리곤)은 다시 투영 행렬로 변환된 후 시각적으로 유효한 클리핑 평면 내부로 클리핑됨 (카메라 좌표계 -> 클립 좌표계)
- 그 후 3D 영역이 화면 공간 좌표계로 변환하기 위해 w 값에 의한 원근 분할과 뷰포트 변환이 적용됨
- 컬링이 활성화된 경우, 컬링 테스트 수행

### Pixel Transfer Operation
- 클라이언트의 메모리를 읽은 뒤, 데이터는 스케일링, bias(?), 매핑 그리고 클램핑됨; 이러한 명령은 `Pixel Transfer Operation`이라 함
- 전송된 데이터는 텍스처 메모리에 저장되고 프래그먼트에 곧바로 래스터라이즈됨

### Texture Memory
- 텍스처 이미지는 기하 객체에 적용되기 위해 텍스처 메모리에 로드됨

### Rasterization
- 래스터라이제이션은 두 기하 정보와 픽셀 데이터가 프래그먼트로 변환되는 과정
- 프래그먼트는 색상, 깊이, 선 두께와 정점 크기와 `antialiasing` 계산 방식(`GL_POINT_SMOOTH`, `GL_LINE_SMOOTH` 그리고 `GL_POLYGON_SMOOTH`)이 담긴 사각 배열
- 만약 쉐이딩 모드가 `GL_FILL`인 경우, 폴리곤 영역 내부는 이때 채워짐
- 각 프래그먼트는 프레임버퍼의 각 픽셀과 대응됨

### Fragment Operation
- 프래그먼트를 프레임버퍼로 변환하는 마지막 과정
- 첫번째로 텍셀 제네레이션; 텍스처 요소는 텍스처 메모리로부터 생성되고 각 프래그먼트에 적용
- 그 뒤 fog(?) 계산 적용
- 그 뒤 여러 개의 프래그먼트 테스트를 순서대로 수행; Scissor Test -> Alpha Test -> Stenicl Test -> Depth Test
- 마지막으로 블랜딩, 디더링, 논리 작업(logical operations)을 수행하고 비트마스크에 의한 마스킹을 수행하여 실제 픽셀 데이터가 프레임버퍼에 저장되도록 함

## OpenGL Transformation
- 3차원 공간 객체는 다음의 과정을 거쳐 2차원 공간으로 그려짐

```
정점 데이터 ==(월드 혹은 로컬 좌표계)>> 모델(위치, 방향, 크기) => 뷰 행렬 ==(카메라 좌표계)>> 프로젝션 행렬 ==(클립 좌표계)>> w에 의해 나누기 ==(NDC 정규화된 공간 좌표계)>> 뷰포트 변환 (화면 좌표계)
```

### 월드 좌표계; Object Coordinates / 모델 좌표계
- glTranslatef, glRotatef, glScalef
- GL에서는 모델 변환과 뷰 변환을 합쳐서 모델 뷰 변환이라 함
- 대상의 로컬 좌표계로 어떠한 변환이 적용되기 이전의 초기 위치와 방향이 담겨있음  
``` 
Mat(model) = Mat(위치/방향, 크기, 이동, 회전 등 기하) X Vec4(객체)
+----------------+   +---+
| sx   0   0  tx |   | x |
|  0  sy   0  ty | X | y |
|  0   0  sy  tz |   | z |
|  0   0   0  1  |   | 1 | => Affine Transformation
+----------------+   +---+
```

### 카메라 좌표계; Eye Coordinates
- gluLookAt
- [OpenGL Utility LookAt 함수 참고](https://arienbv.org/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/)
- 모델뷰 행렬이 월드 좌표계에 곱해진 값으로 로컬 좌표계에서 카메라 공간의 좌표계로 변환된 것 
```
뷰 좌표계 행렬은 아래와 같이 구할 수 있다.
Mat(eye) = Mat(view) X Mat(model)
+----------------+
| sx   0   0  tx |
|  0  sy   0  ty |
|  0   0  sy  tz |
|  0   0   0  1  |
+----------------+
```

### 프로젝션 행렬 및 클립 좌표계
- 카메라 좌표계에서 프로젝션 행렬이 적용되면 클립 좌표계로 변경됨 
- 이 프로젝션 행렬은 프러스텀 frustum으로 보이는 영역을 정의함
- 정점 데이터가 어떤 방식으로 투영될 지 결정; 원근 혹은 직교
- 클립 좌표계로 변경하는 이유는 3차원 좌표가 4차원 좌표 (w)로 비교하기 위해서임

### Normalized Device Coordinates; 정규화된 장치 좌표계
- 각 X, Y, Z축을 w 값으로 나눠 원근 분할을 수행
- 이는 화면(스크린)에 가까운 좌표 변환
- 아직 픽셀로 이동하거나 크기가 적용되지 않은 상태
- 3개의 축은 -1부터 1까지의 크기로 정규화됨

### 뷰포트 변환; Viewport 
- glViewport, glDepthRange
- 정규화된 좌표계가 뷰포트 변환에 의해 정규화하는 과정
- OpenGL 파이프라인의 래스터라이제이션 과정으로 최종적으로 옮기는 과정
- 뷰포트 설정(x, y)과 깊이 버퍼(z)를 통해 이후에 그릴 사각 영역을 결정
- TODO 이 챕터에서 설명한 원리를 코드로 적용해야 함

## OpenGL Projection Matrix
- 카메라 좌표계에서 클립 좌표계로 변환할 때에는 프러스텀을 이용하여 클립 좌표계에 매핑을 시켜야 함
- X, Y, Z축 매핑되는 right left top bottom near far 변수를 통해 카메라 좌표계와 클립 좌표계의 관계를 이해해야 함
- 매핑을 거치면 Z축을 기준으로 동차 좌표계가 반환되고 후에 있을 NDC 계산에서 이들은 3차원 좌표로 변하게 됨
- 카메라 좌표계는 오른손 좌표계인 반면 클립 좌표계는 왼손 좌표계임
- 동차 좌표계로 이루어진 클립 좌표계를 다시 3차원 좌표계로 변환
- OpenGL에서는 반드시 역행렬을 적용해야 함을 잊지 말 것
- TODO 이 챕터에서 설명한 원리를 코드로 적용해야 함

## World, View and Projection Matrix Unveiled
- DirectX 기준 설명
- 행렬, 단위 행렬 간단 설명
- 월드 혹은 모델 매트릭스 (World or Model Matrix)
    - 이동, 회전과 크기와 관련한 정보를 담고 있음
- 뷰 혹은 카메라 매트릭스 (View or Camera Matrix)
    - 카메라 좌표 공간으로 기준을 옮기는 행렬
    - 만약 원근법이 필요하지 않다면 이 변환까지 적용된 행렬로 화면에 표시 가능
- 프로젝션 매트릭스 (Projection Matrix; 투영 행렬)
    - 뷰로부터의 near, far 거리, 화면의 해상도와 카메라의 보는 각도 기반 행렬
- DirectX 기준: matrix4x4 World_View_Projection_Matrix = `World x View x Projection`
- OpenGL 기준: matrix4x4 Projection_View_World_Matrix = `Projection x View x World`

## 참고
- [Cross Product (English Wikipedia)](https://en.wikipedia.org/wiki/Cross_product)
- [Dot Product (English Wikipedia)](https://en.wikipedia.org/wiki/Dot_product)
- [OpenGL Rendering Pipeline (Songho's memos)](http://www.songho.ca/opengl/gl_pipeline.html)
- [OpenGL Transformation (Songho's memos)](http://www.songho.ca/opengl/gl_transform.html)
- [Tutorial Series: Learning How to Write a 3D Soft Engine from Scratch in C#](https://www.davrous.com/2013/06/13/tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-typescript-or-javascript/)
- [Breakdown of the LookAt function in OpenGL](https://arienbv.org/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/)
- [World, View and Projection Matrix Unveiled](https://web.archive.org/web/20131222170415/http:/robertokoci.com/world-view-projection-matrix-unveiled/)
- [Transformations in OpenGL (Bournemouth Univ)](https://nccastaff.bournemouth.ac.uk/jmacey/Lectures/OpenGL/transforms/?print-pdf#/) 
- [Cartesian coordinate system (English Wikipedia)](https://en.wikipedia.org/wiki/Cartesian_coordinate_system)
- [Euler Angles](https://mathworld.wolfram.com/EulerAngles.html)
- [OpenGL Projection Matrix](https://www.songho.ca/opengl/gl_projectionmatrix.html)

## 로드맵
- [로드맵 문서](Roadmap.md)
