#include "Rte/Common.hpp"
#include "SoundBufferImpl.hpp"
#include "SoundImpl.hpp"

#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"

#include <memory>

using namespace Rte::Audio;

void SoundImpl::play() {
    m_sound->play();
}

void SoundImpl::pause() {
    m_sound->pause();
}

void SoundImpl::stop() {
    m_sound->stop();
}

void SoundImpl::setBuffer(const std::shared_ptr<SoundBuffer>& buffer) {
    m_sound = std::make_unique<sf::Sound>(interfaceCast<SoundBufferImpl>(buffer)->getBuffer());
}
