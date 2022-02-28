#include "InstancedModel.h"

InstancedModel::InstancedModel(const std::string& path, int amount) : amount(amount)
{
	modelMatrices.reserve(amount);
	model = new Model(path, glm::vec3(0));
	for (int i = 0; i < amount; i++) modelMatrices.emplace_back(glm::mat4(1.0f));
	init();
}

void InstancedModel::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& viewMatrix)
{
	shader.use();
	shader.setMatrix("projection", projection);
	shader.setMatrix("view", viewMatrix);
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < model->getModelData()->meshes.size(); i++)
	{
		for (int j = 0; j < model->getModelData()->loadedTextures.size(); j++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			int number = 0;
			std::string name = model->getModelData()->loadedTextures[i].type;
			if (name == "texture_diffuse") {
				number = diffuseNr++;
				name = "diffuse";
			}
			else if (name == "texture_specular") {
				number = specularNr++;
				name = "specular";
			}
			shader.setValue<int>("material." + name, number);
			glBindTexture(GL_TEXTURE_2D, model->getModelData()->loadedTextures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(model->getModelData()->meshes[i]->VAO);
		glDrawElementsInstanced(
			GL_TRIANGLES,
			model->getModelData()->meshes[i]->indices.size(),
			GL_UNSIGNED_INT,
			0,
			amount
		);
	}
}

void InstancedModel::setPosition(int index, glm::vec3 position)
{
	if (index < 0 || index >= modelMatrices.size()) throw std::exception("Out of bound");
	modelMatrices[index] = glm::translate(glm::mat4(1.0f), position);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
}

void InstancedModel::init()
{
	// vertex buffer object
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < model->getModelData()->meshes.size(); i++)
	{
		unsigned int VAO = model->getModelData()->meshes[i]->VAO;
		glBindVertexArray(VAO);
		// vertex attributes
		std::size_t vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}
