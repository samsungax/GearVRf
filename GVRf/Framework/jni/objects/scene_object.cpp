/* Copyright 2015 Samsung Electronics Co., LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/***************************************************************************
 * Objects in a scene.
 ***************************************************************************/

#include "scene_object.h"

#include "objects/components/camera.h"
#include "objects/components/camera_rig.h"
#include "objects/components/eye_pointee_holder.h"
#include "objects/components/render_data.h"
#include "util/gvr_log.h"

namespace gvr {
SceneObject::SceneObject() :
        HybridObject(), name_(""), transform_(), render_data_(), camera_(), camera_rig_(), eye_pointee_holder_(), parent_(), children_() {
}

SceneObject::~SceneObject() {
}

void SceneObject::attachTransform(const std::shared_ptr<SceneObject>& self,
        const std::shared_ptr<Transform>& transform) {
    if (transform_) {
        detachTransform();
    }
    std::shared_ptr<SceneObject> owner_object(transform->owner_object());
    if (owner_object) {
        owner_object->detachRenderData();
    }
    transform_ = transform;
    transform_->set_owner_object(self);
}

void SceneObject::detachTransform() {
    if (transform_) {
        transform_->removeOwnerObject();
        transform_.reset();
    }
}

void SceneObject::attachRenderData(const std::shared_ptr<SceneObject>& self,
        const std::shared_ptr<RenderData>& render_data) {
    if (render_data_) {
        detachRenderData();
    }
    std::shared_ptr<SceneObject> owner_object(render_data->owner_object());
    if (owner_object) {
        owner_object->detachRenderData();
    }
    render_data_ = render_data;
    render_data->set_owner_object(self);
}

void SceneObject::detachRenderData() {
    if (render_data_) {
        render_data_->removeOwnerObject();
        render_data_.reset();
    }
}

void SceneObject::attachCamera(const std::shared_ptr<SceneObject>& self,
        const std::shared_ptr<Camera>& camera) {
    if (camera_) {
        detachCamera();
    }
    std::shared_ptr<SceneObject> owner_object(camera->owner_object());
    if (owner_object) {
        owner_object->detachCamera();
    }
    camera_ = camera;
    camera_->set_owner_object(self);
}

void SceneObject::detachCamera() {
    if (camera_) {
        camera_->removeOwnerObject();
        camera_.reset();
    }
}

void SceneObject::attachCameraRig(const std::shared_ptr<SceneObject>& self,
        const std::shared_ptr<CameraRig>& camera_rig) {
    if (camera_rig_) {
        detachCameraRig();
    }
    std::shared_ptr<SceneObject> owner_object(camera_rig->owner_object());
    if (owner_object) {
        owner_object->detachCameraRig();
    }
    camera_rig_ = camera_rig;
    camera_rig_->set_owner_object(self);
}

void SceneObject::detachCameraRig() {
    if (camera_rig_) {
        camera_rig_->removeOwnerObject();
        camera_rig_.reset();
    }
}

void SceneObject::attachEyePointeeHolder(
        const std::shared_ptr<SceneObject>& self,
        const std::shared_ptr<EyePointeeHolder>& eye_pointee_holder) {
    if (eye_pointee_holder_) {
        detachEyePointeeHolder();
    }
    std::shared_ptr<SceneObject> owner_object(
            eye_pointee_holder->owner_object());
    if (owner_object) {
        owner_object->detachEyePointeeHolder();
    }
    eye_pointee_holder_ = eye_pointee_holder;
    eye_pointee_holder_->set_owner_object(self);
}

void SceneObject::detachEyePointeeHolder() {
    if (eye_pointee_holder_) {
        eye_pointee_holder_->removeOwnerObject();
        eye_pointee_holder_.reset();
    }
}

void SceneObject::addChildObject(std::shared_ptr<SceneObject> self,
        std::shared_ptr<SceneObject> child) {
    for (std::shared_ptr < SceneObject > parent = parent_.lock(); parent;
            parent = parent->parent_.lock()) {
        if (child == parent) {
            std::string error =
                    "SceneObject::addChildObject() : cycle of scene objects is not allowed.";
            LOGE("%s", error.c_str());
            throw error;
        }
    }
    children_.push_back(child);
    child->parent_ = self;
    child->transform()->invalidate();
}

void SceneObject::removeChildObject(std::shared_ptr<SceneObject> child) {
    if (child->parent_.lock().get() == this) {
        children_.erase(std::remove(children_.begin(), children_.end(), child),
                children_.end());
        child->parent_.lock().reset();
    }
}

int SceneObject::getChildrenCount() const {
    return children_.size();
}

const std::shared_ptr<SceneObject>& SceneObject::getChildByIndex(int index) {
    if (index < children_.size()) {
        return children_[index];
    } else {
        std::string error = "SceneObject::getChildByIndex() : Out of index.";
        throw error;
    }
}

}
