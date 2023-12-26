//
//  ar_track_vfx_item.cpp
//  MarsNative
//
//  Created by changxing on 2022/9/19.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "ar_track_vfx_item.h"
#include "player/composition.h"

namespace mn {

static const float FACE_UV_DATA[] = {0.276595, 0.653010, 0.276660, 0.622460, 0.276920, 0.592105, 0.278090, 0.561815, 0.280625, 0.531590, 0.285175, 0.501430, 0.292195, 0.471530, 0.301880, 0.442410, 0.314555, 0.414720, 0.330155, 0.388785, 0.348290, 0.364735, 0.368635, 0.342570, 0.390800, 0.322355, 0.414200, 0.303635, 0.439745, 0.287905, 0.468215, 0.277310, 0.499740, 0.273670, 0.531200, 0.277375, 0.559670, 0.288035, 0.585150, 0.303700, 0.608615, 0.322485, 0.630780, 0.342635, 0.651125, 0.364800, 0.669325, 0.388850, 0.684925, 0.414720, 0.697665, 0.442345, 0.707350, 0.471400, 0.714305, 0.501365, 0.718855, 0.531460, 0.721390, 0.561750, 0.722495, 0.592040, 0.722820, 0.622330, 0.722950, 0.652945, 0.329830, 0.678555, 0.358430, 0.703580, 0.392035, 0.707480, 0.426290, 0.702540, 0.457815, 0.692140, 0.456190, 0.673875, 0.424275, 0.681545, 0.391645, 0.685445, 0.359990, 0.684340, 0.541275, 0.692205, 0.572800, 0.702605, 0.607055, 0.707610, 0.640725, 0.703710, 0.669325, 0.678750, 0.639165, 0.684405, 0.607445, 0.685575, 0.574815, 0.681610, 0.542900, 0.673940, 0.499545, 0.642480, 0.499480, 0.602115, 0.499480, 0.561750, 0.499415, 0.522555, 0.452290, 0.499740, 0.475040, 0.494280, 0.499480, 0.488625, 0.523985, 0.494150, 0.546735, 0.499610, 0.362330, 0.636565, 0.380530, 0.648200, 0.402630, 0.652815, 0.426615, 0.647875, 0.445075, 0.632535, 0.424015, 0.626945, 0.401395, 0.624930, 0.380530, 0.628375, 0.554080, 0.632535, 0.572540, 0.647875, 0.596460, 0.652880, 0.618625, 0.648265, 0.636890, 0.636630, 0.618690, 0.628375, 0.597825, 0.624930, 0.575205, 0.626945, 0.413940, 0.428890, 0.445335, 0.442215, 0.481670, 0.448520, 0.499545, 0.446310, 0.517420, 0.448585, 0.553820, 0.442215, 0.585280, 0.428955, 0.563700, 0.404450, 0.535620, 0.386250, 0.499675, 0.379685, 0.463730, 0.386250, 0.435520, 0.404450, 0.423300, 0.427525, 0.457685, 0.431035, 0.499545, 0.429930, 0.541470, 0.431035, 0.575920, 0.427525, 0.542965, 0.413095, 0.499610, 0.406920, 0.456255, 0.413160, 0.403345, 0.639425, 0.595810, 0.639425, 0.084975, 0.420505, 0.282185, 0.122155, 0.500065, 0.024785, 0.717815, 0.122415, 0.915285, 0.420245, 0.220565, 0.846125, 0.778915, 0.846255, 0.499155, 0.851000, 0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 1.000000, 0.000000, 1.000000};

static bool CopyARTrackInfo(MNARTrackInfo& out, MNARTrackInfo* other) {
    if (!other) {
        MLOGE("CopyARTrackInfo input is nullptr");
        return false;
    }
    if (out.key_point_array.size() != other->key_point_array.size()) {
        out.key_point_array.resize(other->key_point_array.size(), 0);
    }
    for (int i = 0; i < out.key_point_array.size(); i++) {
        out.key_point_array[i] = other->key_point_array[i];
    }
    if (out.angle_array.size() != other->angle_array.size()) {
        out.angle_array.resize(other->angle_array.size(), 0);
    }
    for (int i = 0; i < out.angle_array.size(); i++) {
        out.angle_array[i] = other->angle_array[i];
    }
    if ((out.key_point_array.size() % 2 != 0) || (out.angle_array.size() != 3)) {
        MLOGE("CopyARTrackInfo invalid size %ld %ld", out.key_point_array.size(), out.angle_array.size());
        return false;
    }
    float angle_ratio = other->mirror ? 1 : -1;
    out.angle_array[2] *= angle_ratio;
    out.aspect = other->aspect;
    return true;
}

static float CalculatePointDistance(const Vec2& b, const Vec2& c, const float aspect) {
    float x0 = b.m[0];
    float y0 = b.m[1];
    float x1 = c.m[0];
    float y1 = c.m[1];
    float dx = (x1 - x0);
    float dy = (y1 - y0) / aspect;
//    MLOGD("cxdebug1 %f %f %f %f %f %f %f", x0, y0, x1, y1, dx, dy, sqrtf(dx * dx + dy * dy));
    return sqrtf(dx * dx + dy * dy);
}

ARTrackVfxItem::ARTrackVfxItem(ItemData* options, Composition* composition) {
    composition_ = composition;
    
    if (options->type != ITEM_CONTENT_TYPE_PLUGIN) {
        MLOGE("ARTrackVfxItem invalid item type %d", options->type);
        assert(0);
        return;
    }
    if (((PluginContentData*) options->content)->options->type != 1) {
        MLOGE("ARTrackVfxItem invalid plugin type %d", ((PluginContentData*) options->content)->options->type);
        assert(0);
        return;
    }
    
    PluginARContentOptionsData* opt = (PluginARContentOptionsData*) ((PluginContentData*) options->content)->options;
    track_items_count_ = opt->items_count;
    track_items_ref_ = opt->items;
    
    for (int i = 0; i < 3; i++) {
        origin_track_info_.angle_array.push_back(0);
    }
    for (int i = 0; i < 196; i++) {
        origin_track_info_.key_point_array.push_back(1.0f - FACE_UV_DATA[i]);
    }
}

ARTrackVfxItem::~ARTrackVfxItem() {
}

void ARTrackVfxItem::OnConstructed(ItemData* arg) {
}

void ARTrackVfxItem::InnerOnUpdate(float dt, std::shared_ptr<VFXItem> thiz) {
}

BaseContentItem* ARTrackVfxItem::InnerCreateContent() {
    assert(0);
    return nullptr;
}

void ARTrackVfxItem::OnLifeTimeBegin(Composition* renderer, void* content) {
}

void ARTrackVfxItem::UpdateTrackInfo(MNARTrackInfo* info) {
    if (valid_) {
        if (info && (info->key_point_array.size() == 0 || info->angle_array.size() == 0)) {
            MLOGD("ARTrackLoader no data, skip");
            visible_ = false;
            UpdateARTrackItems();
            return;
        }
        valid_ = CopyARTrackInfo(curr_track_info_, info);
        visible_ = true;
        if (!valid_) {
            MLOGE("ARTrackLoader.GetCurrentTrackInfo: invalid info");
            assert(0);
            return;
        }
        UpdateARTrackItems();
    }
}

void ARTrackVfxItem::UpdateARTrackItems() {
    if (track_items_ref_ && composition_) {
        std::vector<std::shared_ptr<VFXItem>>* items = composition_->GetItems();
        for (int i = 0; i < track_items_count_; i++) {
            const auto& item = track_items_ref_[i];
            for (auto& it : *items) {
                if (item.item_id.compare(it->id_) == 0) {
                    Mat4 mat;
                    GetTransformOffset(mat, item.track_index, item.track_start, item.track_end);
                    CalculateItem* content = ((CalculateItem*) it->content_);
                    if (content) {
                        if (!content->custom_transform_) {
                            content->custom_transform_ = new Transform;
                        }
                        content->custom_transform_->FromMat4(mat);
                    }
                    break;
                }
            }
        }
    }
}

void ARTrackVfxItem::GetTransformOffset(Mat4& out, int p0, int p1, int p2) {
    // 计算原始三角形位置、旋转角度
    Vec2 origin_pos[3];
    origin_pos[0].Set(origin_track_info_.key_point_array[p0 * 2], 1.0f - origin_track_info_.key_point_array[p0 * 2 + 1]);
    origin_pos[1].Set(origin_track_info_.key_point_array[p1 * 2], 1.0f - origin_track_info_.key_point_array[p1 * 2 + 1]);
    origin_pos[2].Set(origin_track_info_.key_point_array[p2 * 2], 1.0f - origin_track_info_.key_point_array[p2 * 2 + 1]);
    {
        float scaled_height = 1.0f / curr_track_info_.aspect;
        float scaled_start_y = (scaled_height - 1.0f) * 0.5f;
        float scaled_end_y = scaled_height - scaled_start_y;
        for (int i = 0; i < 3; i++) {
            origin_pos[i].m[1] = (scaled_start_y + (scaled_end_y - scaled_start_y) * origin_pos[i].m[1]) / scaled_height;
        }
    }
    Vec3 origin_angle(origin_track_info_.angle_array[0], origin_track_info_.angle_array[1], origin_track_info_.angle_array[2]);
    // 计算当前三角形位置、旋转角度
    Vec2 curr_pos[3];
    Vec3 curr_angle;
    Vec4 quad;
    Vec3 pos;
    Vec3 scale(1, 1, 1);

    float camera_z = composition_->camera_->GetPositionZ();
    float camera_fov = composition_->camera_->GetFOV();
    float ratio = 2 * camera_z * std::tan(camera_fov / 2 * M_PI / 180) + 1e-6;

    // 如果数据出现过异常，隐藏贴纸
    // 如果人脸不可见，隐藏贴纸
    if ((!valid_) || (!visible_)) {
        pos.Set(10 * ratio, 10 * ratio, 0);
        MathUtil::Mat4FromRotationTranslationScale(out, quad, pos, scale);
    } else {
        curr_pos[0].Set(curr_track_info_.key_point_array[p0 * 2], 1.0f - curr_track_info_.key_point_array[p0 * 2 + 1]);
        curr_pos[1].Set(curr_track_info_.key_point_array[p1 * 2], 1.0f - curr_track_info_.key_point_array[p1 * 2 + 1]);
        curr_pos[2].Set(curr_track_info_.key_point_array[p2 * 2], 1.0f - curr_track_info_.key_point_array[p2 * 2 + 1]);
        curr_angle.Set(curr_track_info_.angle_array[0], curr_track_info_.angle_array[1], curr_track_info_.angle_array[2]);

        if (!curr_track_info_.mirror) {
            curr_angle.m[2] = -curr_angle.m[2];
        }
        // 计算两个顶点的距离，得到缩放
        float origin_distance = CalculatePointDistance(origin_pos[1], origin_pos[2], curr_track_info_.aspect);
        float curr_distance = CalculatePointDistance(curr_pos[1], curr_pos[2], curr_track_info_.aspect);

        Vec4 quad;
        MathUtil::QuatFromRotation(quad,
                                (curr_angle.m[0] - origin_angle.m[0]),
                                -(curr_angle.m[1] - origin_angle.m[1]),
                                (curr_angle.m[2] - origin_angle.m[2]));
    //    printf("cxdebug %f %f %f %f %f %f\n", origin_distance, curr_distance, curr_pos[1].m[0], curr_pos[1].m[1], curr_pos[2].m[0], curr_pos[2].m[1]);
        Vec3 pos((curr_pos[0].m[0] - origin_pos[0].m[0]) * ratio, (curr_pos[0].m[1] - origin_pos[0].m[1]) * ratio / curr_track_info_.aspect, 0);
        Vec3 scale(curr_distance / origin_distance, curr_distance / origin_distance, curr_distance / origin_distance);
    //    Vec3 scale(1, 1, 1);
        MathUtil::Mat4FromRotationTranslationScale(out, quad, pos, scale);
    }
}

}
