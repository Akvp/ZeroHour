#include "Main.h"

void CMain::OnRender()
{
	//Clear the screen for each frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use shaders
	mainProgram.use();

	mainProgram.SetUniform("matrices.projMatrix", Projection);
	mainProgram.SetUniform("matrices.viewMatrix", View);
	mainProgram.SetUniform("gSampler", 0);

	mainProgram.SetUniform("matrices.modelMatrix", Model);
	mainProgram.SetUniform("matrices.normalMatrix", glm::mat4(1.0));
	mainProgram.SetUniform("vColor", glm::vec4(1, 1, 1, 1));
	
	sun.setUniform(&mainProgram, "sunLight");

	mainProgram.SetUniform("matrices.modelMatrix", glm::translate(glm::mat4(1.0), Position));
	skybox.render();

	mainProgram.SetUniform("matrices.modelMatrix", glm::mat4(1.0));

	//Render ground
	glBindVertexArray(scene_VAO);
	scene_texture.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Render models
	CModel::bindVAO();
	float xPosHouse = -80.0f + 30.0f;
	glm::mat4 mModel = glm::translate(glm::mat4(1.0), glm::vec3(xPosHouse, 0, 0));
	mModel = glm::scale(mModel, glm::vec3(3, 3, 3));
	mainProgram.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
	models[0].render();
	//models[1].render();

	SDL_GL_SwapWindow(Window_Main);
}