#include "main.h"

bool prepareShaderProgram( QGLShaderProgram* shader, const QString& vertexShaderPath, const QString& fragmentShaderPath )
{
  // First we load and compile the vertex shader…
  bool result = shader->addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );

  if ( !result )
    qWarning() << shader->log();

  // …now the fragment shader…
  result = shader->addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
  if ( !result )
    qWarning() << shader->log();

  // …and finally we link them to resolve any references.
  result = shader->link();
  if ( !result )
    qWarning() << "Could not link shader program:" << shader->log();

  return result;
}
