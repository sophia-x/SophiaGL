#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <tools/obj_loader>

namespace ObjLoader
{

void load_obj(const string& path, vector<unsigned short>& indices, vector<vec3>& vertices, vector<vec2>& uvs, vector<vec3>& normals) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_SortByPType);
	if (!scene || !scene->HasMeshes()) {
		fprintf(stderr, "%s", importer.GetErrorString());
		throw "Load obj: can not load obj error";
	}

	const aiMesh* mesh = scene->mMeshes[0];

	// Fill vertices positions
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D pos = mesh->mVertices[i];
		vertices.push_back(vec3(pos.x, pos.y, pos.z));
	}

	// Fill vertices texture coordinates
	if (mesh->HasTextureCoords(0)) {
		uvs.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D uvw = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords;
			uvs.push_back(vec2(uvw.x, uvw.y));
		}
	} else {
		uvs = vector<vec2>(mesh->mNumVertices, vec2(0));
	}

	// Fill vertices normals
	normals.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D n = mesh->mNormals[i];
		normals.push_back(vec3(n.x, n.y, n.z));
	}

	// Fill face indices
	indices.reserve(3 * mesh->mNumFaces);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		if (mesh->mFaces[i].mNumIndices != 3)
			continue;

		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
}

void computeTangentBasis(
    // inputs
    const vector<unsigned short>& indices,
    const vector<vec3>& vertices,
    const vector<vec2>& uvs,
    const vector<vec3>& normals,
    // outputs
    vector<vec3>& tangents,
    vector<vec3>& bitangents
) {

	for (unsigned int i = 0; i < indices.size(); i += 3 ) {
		// Shortcuts for vertices
		const vec3& v0 = vertices[indices[i]];
		const vec3& v1 = vertices[indices[i + 1]];
		const vec3& v2 = vertices[indices[i + 2]];

		// Shortcuts for UVs
		const vec2& uv0 = uvs[indices[i]];
		const vec2& uv1 = uvs[indices[i + 1]];
		const vec2& uv2 = uvs[indices[i + 2]];

		// Edges of the triangle : postion delta
		vec3 edge_ab = v1 - v0;
		vec3 edge_ac = v2 - v0;

		// UV delta
		vec2 delta_UV_a = uv1 - uv0;
		vec2 delta_UV_b = uv2 - uv0;

		float r = 1.0f / (delta_UV_a.x * delta_UV_b.y - delta_UV_a.y * delta_UV_b.x);
		vec3 tangent = normalize((edge_ab * delta_UV_b.y   - edge_ac * delta_UV_a.y) * r);
		vec3 bitangent = normalize((edge_ac * delta_UV_a.x   - edge_ab * delta_UV_b.x) * r);

		tangents[indices[i]] += tangent;
		bitangents[indices[i]] += bitangent;
		tangents[indices[i + 1]] += tangent;
		bitangents[indices[i + 1]] += bitangent;
		tangents[indices[i + 2]] += tangent;
		bitangents[indices[i + 2]] += bitangent;

	}

	for (unsigned int i = 0; i < vertices.size(); i ++) {
		const vec3& n = normals[i];
		vec3& t = tangents[i];
		const vec3& b = bitangents[i];

		// Gram-Schmidt orthogonalize
		t = normalize(t - n * dot(n, t));

		// Calculate handedness
		if (dot(cross(n, t), b) < 0.0f) {
			t = t * -1.0f;
		}
	}
}

}